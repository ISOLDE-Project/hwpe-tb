#include "Vtop__Dpi.h"
#include "Vtop.h"
#include "lib/elfLoader.hpp"
#include <cstdint>
#include <map>

typedef std::map<long long, long long> storage_map_type;

// extern ELFLoader loader;
extern uint32_t* elf_data;
storage_map_type g_storage;
storage_map_type::iterator g_read_it;


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
        printf("\renodeDPIReceive\n");
        if(g_read_it != g_storage.end()){
            //printf("read will be performed from initialised address=0x%x", *address)
            *address = g_read_it->first;
            *data    = g_read_it->second;            
        } else {
            printf("read is performed from un-initialised address=0x%llx", *address);
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
        printf("\renodeDPISendToAsync, action=%d,address=0x%llx, data=0x%llx\n", action, address,data);
        printf("Elf.data: 0x%x\n", *(elf_data + (uint32_t)address));
        switch( action){
            default:
                printf("\renodeDPISendToAsync, unknowned action=%d, for address=0x%llx, data=0x%llx\n", action, address,data);
                break;
            case 13: //write
                {
                    storage_map_type::iterator it = g_storage.find(address);
                    if(it != g_storage.end()){
                        //
                        g_storage[address]=data;
                        printf("updating address=0x%llx with data=0x%llx\n", address, data);
                    } else {
                        g_storage.insert(std::pair<long long, long long>(address,data));
                        printf("writting at address=0x%llx  data=0x%llx\n", address, data);
                    }
                    
                }
                break;
            case 14: //read
                {   
                    g_read_it = g_storage.find(address);
                    if(g_read_it != g_storage.end()){
                        printf("read will be performed from initialised address=0x%llx\n", address);
                    } else {
                        printf("read will be performed from un-initialised address=0x%llx\n", address);
                    }
                        
                }
                break;
            };

        return 1;
    }