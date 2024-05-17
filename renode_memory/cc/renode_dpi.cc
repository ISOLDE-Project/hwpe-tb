// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

#include "Vtop__Dpi.h"
#include "Vtop.h"
#include <map>

typedef long long addr_type;
typedef long long data_type;
typedef std::map<addr_type, data_type> storage_type;

struct RX_data{
    RX_data():action(0),address(0),data(0){}
int action;
addr_type address;
data_type data;
};

storage_type g_storage;
storage_type::iterator g_read_it;

//
RX_data g_rx;


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

        printf("renodeDPIReceive: ");
        if(g_read_it != g_storage.end()){
            //printf("read will be performed from initialised address=0x%x", *address)
            *address = g_read_it->first;
            *data    = g_read_it->second;            
        } else {
            printf("\n WARNING: (%d) value type is read from an un-initialised address=0x%llx\n", *action, *address);
            *data = static_cast<long long>( 0);
        }
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
        g_rx.action = action;
        g_rx.address = address;
        g_rx.data = data;

        printf("\renodeDPISendToAsync, action=%d,address=0x%llx, data=0x%llx\n", action, address,data);
        switch( action){
            default:
                printf("renodeDPISendToAsync, unknown action=%d, for address=0x%llx, data=0x%llx\n", action, address,data);
                break;
            case 13: // pushWord = 13,
                {
                    storage_type::iterator it = g_storage.find(address);
                    if(it != g_storage.end()){
                        //
                        g_storage[address]=data;
                        printf("updating address=0x%llx with Word=0x%llx\n", address, data);
                    } else {
                        g_storage.insert(std::pair<long long, long long>(address,data));
                        printf("writting at address=0x%llx  Word=0x%llx\n", address, data);
                    }
                    
                }
                break;
            case 29: //pushQuadWord = 29,
                {
                    storage_type::iterator it = g_storage.find(address);
                    if(it != g_storage.end()){
                        //
                        g_storage[address]=data;
                        printf("updating address=0x%llx with QuadWord=0x%llx\n", address, data);
                    } else {
                        g_storage.insert(std::pair<long long, long long>(address,data));
                        printf("writting at address=0x%llx  QuadWord=0x%llx\n", address, data);
                    }

                }
                break;
////////////////////////////////////////////////////
            case 14: //getWord = 14,
                {   
                    g_read_it = g_storage.find(address);
                    if(g_read_it != g_storage.end()){
                        printf("Word read wil from initialised address=0x%llx\n", address);
                    } else {
                        printf("Word read will be performed from un-initialised address=0x%llx\n", address);
                    }
                        
                }
                break;
            case 30: //getQuadWord = 30,
                {
                    g_read_it = g_storage.find(address);
                    if(g_read_it != g_storage.end()){
                        printf("QuadWord read will be performed from initialised address=0x%llx\n", address);
                    } else {
                        printf("QuadWord read will be performed from un-initialised address=0x%llx\n", address);
                    }
                }
                break;
        }



        return 1;
    }
