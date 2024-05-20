#pragma once


#include "elfFile.h"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>
#include "trace.hpp"




const size_t ROM_ORIGIN = 0x00001000;

struct ELFLoader {

  unsigned IDX_MAX = 0;
  uint32_t startAddress;
  typedef std::vector<uint32_t> storage_type;
  storage_type storage;

  ELFLoader() {  }
  unsigned max_idx(){ return IDX_MAX;}
  uint32_t* getStorage(){return storage.data();}

  FILE* fopenCheck(const char* fname, const char* mode)
  {
    FILE* fp = fopen(fname, mode);
    if (fp == NULL)
    {
      fprintf(stderr, "Error: cannot open file %s\n", fname);
      exit(-1);
    }
    return fp;
  }

  FILE* openOrDefault(const std::string fname, const char* mode, FILE* def)
  {
    return (fname.empty()) ? def : fopenCheck(fname.c_str(), mode);
  }

  uint32_t getStartAddress(){ return startAddress;}
 

  /**
  ** expands elf content value from 8 bit to 32 bit
  * while keeping the address in 32 bit range
  **
  * it reads for consecutive bytes and store them at the address of first byte
  */

  void setByte(const unsigned addr, const uint8_t value)
  {
    unsigned idx = (addr-ROM_ORIGIN) >> 2;
    if (IDX_MAX < idx)
      IDX_MAX = idx;
    if (idx > storage.capacity())
    {
      fprintf(stderr, "addr=0x%X,idx=0x%X,allocated_memory=0x%zX\n", addr, idx, storage.capacity());
      assert(idx < storage.capacity() && "Allocated memory doesn't fit the RISCV code");
    }
    DataUnion tmp;
    tmp.ui32 = storage[idx];
    tmp.ui8[addr % 4]= value;
    storage[idx] = tmp.ui32;
  }

uint32_t computeImageSize(ElfFile& elfFile){
  uint32_t section_end=0;
    for (const auto& section : elfFile.sectionTable)
    {
      if (section.address != 0 && section.address >= ROM_ORIGIN){
        auto tmp = section.address+section.size;
        if(tmp>section_end)
          section_end=tmp;
      }
    }      

    uint32_t result = section_end>ROM_ORIGIN?(section_end-ROM_ORIGIN)/4:0;
    result+=1;
    fprintf(stderr, "first.section.start=0x%zX, last.section.end=0x%X, allocated_memory=%d\n", ROM_ORIGIN, section_end, result);
    return result;
}

  void readElf(std::string& binaryFile)
  {
    ElfFile elfFile(binaryFile.c_str());
    auto capacity=computeImageSize(elfFile);
    storage.resize(capacity);
    for (const auto& section : elfFile.sectionTable)
    {
      if (section.address != 0 && section.address >= ROM_ORIGIN)
      {
        
        //fprintf(stderr, "section.type=%d, section.start=0x%X, section.size=0x%X,section.end=0x%X, section.name%s\n",
        //        section.type, section.address, section.size,section.address+section.size, section.name.c_str());
        if( SHT_PROGBITS!= section.type)
            continue;
        for (unsigned i = 0; i < section.size; i++){
          auto pos = section.offset + i;
          if(pos < elfFile.content.size()){
            auto input=elfFile.content[pos];
            setByte(section.address + i, input);
          }else{
            fprintf(stderr, "WARNING: %s.end = %d > ElfFileContent= %zu\n",
                            section.name.c_str(), section.address+section.size, elfFile.content.size());
            break;
          }
        }              
      }
    }
    startAddress = find_by_name(elfFile.symbols, "_start").offset;
  
    storage.push_back( 0xCADEBABA);
    storage.push_back( 0xACDCFACE);
    IDX_MAX=IDX_MAX+3;
   
    fprintf(stderr, "max_idx() =%d, storage.size()=%zu\n", max_idx(),storage.size());
  
  }
};



template <typename elem_type, typename container_type>
void py_pretty_print(FILE* trace, std::string& name, container_type& container)
{
  // auto size=container.size();
  auto size = container.storage.size();
  fprintf(trace, "%s = np.array([\n", name.c_str());
  for (auto w = 0; w < size; ++w)
  {
    elem_type elem;
    elem = container.storage[w];
    fprintf(trace, py_print_format<elem_type>(w, size), elem);
  }
  fprintf(trace, "],dtype=%s)\n", get_npType<elem_type>());
}

template <typename elem_type, typename container_type>
void csv_pretty_print(FILE* trace, std::string& name, container_type& container)
{
  // auto size=container.size();
  auto size = container.max_idx();
  fprintf(trace, "idx,address,value\n");
  for (auto w = 0; w < size; ++w)
  {
    elem_type elem;
    elem = container.storage[w];
    fprintf(trace, c_print_format<elem_type>(w, w + 2), w);      // force  comma
    fprintf(trace, c_print_format<elem_type>(w, w + 2), w << 2); // force  comma
    fprintf(trace, c_print_format<elem_type>(w, w + 1), elem);   // force no comma
    fprintf(trace, "\n");
  }
}
