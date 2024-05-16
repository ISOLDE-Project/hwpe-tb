#ifndef TRACE_UTILITIES
#define TRACE_UTILITIES


#include <cmath>
#include <cstdint>
#include <cstdio>

#include "logger.hpp"

#ifndef __MEMORY_INTERFACE_H__

union DataUnion{
  float f32;
  uint32_t ui32;
  int32_t i32;
  uint16_t ui16[2];
  uint8_t ui8[4];
};
#endif

inline void dump( FILE* trace,  int32_t* ptr,  uint32_t size=4){
    fprintf(trace,"[ %d ",ptr[0]);     
    for(int i=1;i<size;++i){
         fprintf(trace,", %d",ptr[i]);     
    }
    fprintf(trace," ]\n ");     
}


inline void dump( FILE* trace,  uint32_t* ptr,  uint32_t size=4){
    fprintf(trace,"[ %u ",ptr[0]);     
    for(int i=1;i<size;++i){
         fprintf(trace,", %u",ptr[i]);     
    }
    fprintf(trace," ]\n ");     
}

inline void dump( FILE* trace,  const uint32_t* ptr,  uint32_t size=4){
    fprintf(trace,"[ %u ",ptr[0]);     
    for(int i=1;i<size;++i){
         fprintf(trace,", %u",ptr[i]);     
    }
    fprintf(trace," ]\n ");     
}
inline void dump( FILE* trace,  float* ptr,  uint32_t size=4){ 
    fprintf(trace,"[ %f ",ptr[0]);     
    for(int i=1;i<size;++i){
         fprintf(trace,", %f",ptr[i]);     
    }
    fprintf(trace," ]\n ");     

}

namespace{


  template <typename elem_type>
  const char* pretty_print_format();

  template<>
  inline const char* pretty_print_format<float>() {return "\t\t\t\t%f\n";}

  template<>
  inline const char* pretty_print_format<int32_t>() {return "\t\t\t\t%d\n";}

  template<>
  inline const char* pretty_print_format<uint32_t>() {return "\t\t\t\t%u\n";}

  template <typename elem_type> 
  inline const char*get_npType();

  template<>
  inline const char* get_npType<float>() {return " np.float32" ;}

  template<>
  inline const char* get_npType<int32_t>() {return " np.int32 ";}

  template<>
  inline const char* get_npType<uint32_t>() {return " np.uint32 ";}

  template <typename elem_type>
  const char* py_print_format(uint32_t i, uint32_t max);

  template <>
  inline const char* py_print_format<float>(uint32_t i, uint32_t max){
    bool last =(i==max-1);
    if(last)
        return "%f";
    return "%f, ";
  }

  template <>
  inline const char* py_print_format<int32_t>(uint32_t i, uint32_t max){
    bool last =(i==max-1);
    if(last)
        return "%d";
    return "%d, ";
  }
  template <>
  inline const char* py_print_format<uint32_t>(uint32_t i, uint32_t max){
    bool last =(i==max-1);
    if(last)
        return "%u";
    return "%u, ";
  }

//
  template <typename elem_type>
  const char* c_print_format(uint32_t i, uint32_t max);

  template <>
  inline const char* c_print_format<uint32_t>(uint32_t i, uint32_t max){
    bool last =(i==max-1);
    if(last)
        return "0x%X";
    return "0x%X, ";
  }

}



#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
template<typename elem_type,typename addr_type,typename shape_type, typename io_type>
void pretty_print(FILE* trace , const addr_type ptr_w,const shape_type& shape_w, io_type& io){
  
    fprintf(trace,"[\n");
    for (uint32_t m=0;m<shape_w[0];++m){
        fprintf(trace,"\t[\n");
        for (uint32_t c=0;c<shape_w[1];++c){         
                fprintf(trace,"\t\t[\n");
                for (uint32_t h=0;h<shape_w[2];++h){
                    fprintf(trace,"\t\t\t[\n");
                    for (uint32_t w=0;w<shape_w[3];++w){
                        shape_type index;
                        index={m,c,h,w};
                        //addr = ptr_w+offset;
                        //core.im->fetchData(addr,value);
                        elem_type elem;
#if 0                        
                        io.tensor_read(ptr_w, shape_w, index,elem);
#else
                        elem = io.template tensor_read<elem_type>((elem_type*)ptr_w, shape_w, index);
#endif                        
                        fprintf(trace,pretty_print_format<elem_type>(),elem);
                        //offset+=sizeof(pType);
                    }
                    fprintf(trace,"\t\t\t]\n");
                }
                fprintf(trace,"\t\t]\n");
        }
        fprintf(trace,"\t]\n");
    }
    fprintf(trace,"]\n");
}


template<typename elem_type,typename addr_type,typename shape_type, typename io_type>
void py_pretty_print(FILE* trace ,std::string& name, const addr_type ptr_w,const shape_type& shape_w, io_type& io,volatile uint32_t* mem){
  using index_t = int32_t;
    fprintf(trace,"%s = np.array([\n", name.c_str());
    for (index_t m=0;m<shape_w[0];++m){
        fprintf(trace,"\t[\n");
        for (index_t c=0;c<shape_w[1];++c){         
                fprintf(trace,"\t\t[\n");
                for (index_t h=0;h<shape_w[2];++h){
                    fprintf(trace,"\t\t\t[  ");
                    for (index_t w=0;w<shape_w[3];++w){
                        shape_type index;
                        index={m,c,h,w};
                        //addr = ptr_w+offset;
                        //core.im->fetchData(addr,value);
                        elem_type elem;
                        elem = io.template tensor_read<elem_type>(mem,ptr_w, shape_w, index);
                        fprintf(trace,py_print_format<elem_type>(w,shape_w[3]),elem);
                        //offset+=sizeof(pType);
                    }
                    bool last = (h == shape_w[2]-1);
                    if(last)
                        fprintf(trace,"  ]\n");
                    else
                        fprintf(trace,"  ],\n");
                }
                //fprintf(trace,"\t\t]\n");
                bool last = (c == shape_w[1]-1);
                if(last)
                    fprintf(trace,"\t\t]\n");
                else
                    fprintf(trace,"\t\t],\n");
        }
        //fprintf(trace,"\t]\n");
        bool last = (m == shape_w[0]-1);
        if(last)
            fprintf(trace,"\t]\n");
        else
            fprintf(trace,"\t],\n");

    }
    fprintf(trace,"],dtype=%s)\n",get_npType<elem_type>());
}
#pragma GCC diagnostic pop

template<typename elem_type>
bool are_equal(elem_type a, elem_type b){
    float diff =((float)a-float(b))/(float)b;
    diff=std::abs(diff);
    bool res = true;//diff<1.0? true: false;
    //auto diff= std::abs(a-b);
    //const auto accuracy =elem_type(20);
    //bool res = diff<accuracy ? true: false;
    return res;

}

template<>
inline bool are_equal<float>(float a, float b){
    auto diff= std::abs(a-b);
#ifdef _NO_COSIM_
const float accuracy = 1e-3;
#pragma message "_NO_COSIM_"
#else
const float accuracy = 1e-3;
#endif    
    bool res = diff<accuracy? true: false;
    return res;
}

namespace{
    struct Diff_Logfile{
        static constexpr char name[] = "diff.csv";
    };
    using Diff= conti::Logger<Diff_Logfile>;
}

template<typename elem_type,typename addr_type,typename shape_type, typename io_type, typename testfunc>
bool check( const addr_type actual , const addr_type expected,const shape_type& shape_w, io_type& io, testfunc& are_equal_func){
    bool res =true;
    std::ostringstream stream;
    stream<< "diff,actual,expected";
    Diff::getInstance().header(stream);    
    for (uint32_t m=0;m<shape_w[0];++m){
        for (uint32_t c=0;c<shape_w[1];++c){         
                for (uint32_t h=0;h<shape_w[2];++h){
                    for (uint32_t w=0;w<shape_w[3];++w){
                        shape_type index;
                        index.set(m,c,h,w);
                        elem_type actual_elem,expected_elem;
                        actual_elem   = io.template tensor_read<elem_type>((elem_type*)actual,   shape_w, index);
                        expected_elem = io.template tensor_read<elem_type>((elem_type*)expected, shape_w, index);
                        if(res)
                            res = are_equal_func(actual_elem,expected_elem);
                        //if(!res) return res;  
                        std::ostringstream stream;
                        stream<< (actual_elem - expected_elem)<<", "<<actual_elem<<", "<<expected_elem;
                        Diff::getInstance().info(stream);                                              

                    }
                }
        }
    }
    return res;
}

template<typename elem_type,typename addr_type,typename shape_type, typename io_type>
bool check_tensor_col(  elem_type* expected_vector , const addr_type actual_tensor,const shape_type& shape_w,uint32_t ix, io_type& io){
    bool res;
    
    for (uint32_t m=0;m<shape_w[0];++m){
        for (uint32_t c=0;c<shape_w[1];++c){         
                for (uint32_t h=0;h<shape_w[2];++h){
                    //for (uint32_t w=0;w<shape_w[3];++w){
                        shape_type index;
                        index={m,c,h,ix};
                        elem_type actual_elem,expected_elem;
                        expected_elem   = expected_vector[h];//io.template tensor_read<elem_type>((elem_type*)actual,   shape_w, index);
                        actual_elem     = io.template tensor_read<elem_type>((elem_type*)actual_tensor, shape_w, index);
                        res = are_equal(actual_elem,expected_elem);
                        if(!res) return res;                                                 
                    //}
                }
        }
    }
    return res;
}

template<typename elem_type>
bool check_vector(  elem_type* actual_vector , elem_type* expected_vector,uint32_t size){
    bool res;
    for (uint32_t h=0;h<size;++h){
        
            elem_type actual_elem,expected_elem;
            actual_elem   = actual_vector[h];
            expected_elem = expected_vector[h];
            res = are_equal(actual_elem,expected_elem);
            if(!res) return res;                                                 
        
    }
    return res;
}

#endif