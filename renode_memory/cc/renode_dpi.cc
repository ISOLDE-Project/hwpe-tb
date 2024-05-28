// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

#include "Vtop__Dpi.h"
#include "Vtop.h"
#include <map>

#include "MemorySim.hh"

typedef long long addr_type;
typedef long long data_type;
//typedef std::map<addr_type, data_type> storage_type;

struct RX_data{
    RX_data():action(0),address(0),data(0){}
int action;
addr_type address;
data_type data;
};

//storage_type g_storage;
//storage_type::iterator g_read_it;
union DataUnion64{
  uint64_t ui64;
  uint32_t ui32[2];
};
//
RX_data g_rx;

static const char* decodeAction(int action){
    switch(action){
        default:
            return "unknown";
        case 13:
        case 14:
            return "Word";
        case 11:
        case 12:
            return "DoubleWord";
        case 29:
        case 30:
            return "QuadWord";
        

    }
}

    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:39:32
    extern void renodeDPIConnect(int receiverPort, int senderPort, const char* address){
        printf("\nrenodeDPIConnect\n");
    }
    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:45:32
    extern void renodeDPIDisconnect(){
        printf("\renodeDPIDisconnect\n");
    }
    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:47:31
    extern svBit renodeDPIIsConnected(){
        printf("\renodeDPIIsConnected\n");
        return 1;
    }
    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:49:32
    extern void renodeDPILog(int logLevel, const char* data){
        printf("\renodeDPILog:%s\n",data);
    }
    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:54:31
    extern svBit renodeDPIReceive(int* action, long long* address, long long* data){
        //copy the "received data"
        *action  = g_rx.action;
        *address = g_rx.address;
        *data    = g_rx.data;
        ISOLDE::DataUnion src;
        bool fetchStatus = ISOLDE::MemorySim::fetchData(g_rx.address,src);
        switch( g_rx.action){
            default:
                printf("renodeDPISendToAsync, unknown action=%d, for address=0x%llx, data=0x%llx\n", *action, *address,*data);
                return 0;
            case 12: //getDoubleWord = 12,
                  *data = static_cast<data_type>( src.ui32);
                  break;
            case 14: //getWord = 14,
                { 
                  int slot = *address % 4?1:0;
                  *data = static_cast<data_type>( src.ui16[slot]);
                }
                  break;
            case 30: //getQuadWord = 30,
                {
                    ISOLDE::DataUnion src;                   
                    DataUnion64 dst;
                    ISOLDE::MemorySim::fetchData(g_rx.address,src);
                    dst.ui32[0] =src.ui32;
                    ISOLDE::MemorySim::fetchData(g_rx.address+4,src);
                    dst.ui32[1] =src.ui32;
                     *data = dst.ui64;
                }
                  break;
        };

        printf("renodeDPIReceive: status= %s, value =0x%llx  @address=0x%llx\n", fetchStatus?"OK":"FAILED",*data,*address);
        return 1;

    }

    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:60:31
    extern svBit renodeDPISend(int action, long long address, long long data){
        printf("\renodeDPISend\n");
        return 1;
    }
    // DPI import at /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:66:31
    extern svBit renodeDPISendToAsync(int action, long long address, long long data){
        // simulate sending and reception
        g_rx.action = 0;
        switch( action){
            default:
                printf("renodeDPISendToAsync, unknown action=%d, for address=0x%llx, data=0x%llx\n", action, address,data);
                return 0;
            case 11: //pushDoubleWord = 11,
                { 
                    ISOLDE::DataUnion src;
                    src.ui32= static_cast<uint32_t>(data);
                    ISOLDE::MemorySim::pushData(address,src);
                }
                break;
            case 13: //pushWord = 13,
                { 
                    ISOLDE::DataUnion src;
                    ISOLDE::MemorySim::fetchData(address,src);
                    int slot = address % 4?1:0;
                    src.ui16[slot]= static_cast<uint16_t>(data);
                    ISOLDE::MemorySim::pushData(address,src);
                }
                break;
            case 29: //pushQuadWord = 29,
                {
                                    
                    ISOLDE::DataUnion dst;                   
                    DataUnion64 src;
                    src.ui64=data;
                    dst.ui32 = src.ui32[0];
                    ISOLDE::MemorySim::pushData(address,dst);
                    dst.ui32 = src.ui32[1];
                    ISOLDE::MemorySim::pushData(address+4,dst);
                
                }
                break;
////////////////////////////////////////////////////
            case 12: //getDoubleWord = 12,
            case 14: //getWord = 14,
            case 30: //getQuadWord = 30,
                {   
                    g_rx.action = action;
                    g_rx.address = address;
                    g_rx.data = data;
                                    
                }
                break;
        }



        return 1;
    }
