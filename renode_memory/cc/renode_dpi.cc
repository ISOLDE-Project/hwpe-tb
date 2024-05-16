#include "Vtop__Dpi.h"
#include "Vtop.h"
#include "lib/elfLoader.hpp"
#include <cstdint>
#include <map>

typedef std::map<long long, long long> storage_map_type;
static constexpr uint32_t ROM_RN_ORIGIN = 0;//0x00001000;
// extern ELFLoader loader;
extern uint32_t* elf_data;
unsigned read_idx;
extern unsigned MAX_IDX;
storage_map_type g_storage;
storage_map_type::iterator g_read_it;

    static inline uint32_t addr_to_index(const uint32_t addr){
        uint32_t tmp_sh_addr = addr - ROM_RN_ORIGIN; //declared in elfLoader
        return static_cast<uint32_t>(tmp_sh_addr>>2);
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
        printf("\renodeDPIReceive\n");
        // if(g_read_it != g_storage.end()){
        // unsigned idx = addr_to_index((uint32_t)*address);
        if(read_idx < MAX_IDX){
            // printf("read will be performed from initialised address=0x%x\n", *address);
            // uint32_t *tmp_addr = elf_data + read_idx;
            // *address = *tmp_addr;
            *data = *(elf_data + read_idx);            
        } else {
            printf("read is performed from un-initialised address=0x%llx\n", *address);
            *data = static_cast<uint32_t>(0);
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
        // printf("Elf.data: 0x%x\n", *(elf_data + (uint32_t)address));
        // printf("idx: 0x%llx\n", addr_to_index((uint32_t)address));
        // printf("Elf.data: 0x%x\n", elf_data[addr_to_index((uint32_t)address)]);
        printf("Elf.data: 0x%x\n", *(elf_data + addr_to_index((uint32_t)address)));
        switch( action){
            default:
                printf("\renodeDPISendToAsync, unknowned action=%d, for address=0x%llx, data=0x%llx\n", action, address,data);
                break;
            case 13: //write
                {

                    // storage_map_type::iterator it = g_storage.find(address);
                    unsigned idx = addr_to_index((uint32_t)address);
                    if(idx < MAX_IDX){
                        //
                        // g_storage[address]=data;
                        uint32_t *tmp_addr = elf_data + idx;
                        *tmp_addr = (uint32_t)data;
                        printf("updating address=0x%llx with data=0x%llx\n", address, data);
                    } else {

                        // g_storage.insert(std::pair<long long, long long>(address,data));
                        printf("writting at address=0x%llx  data=0x%llx\n", address, data);
                    }
                    
                    // storage_map_type::iterator it = g_storage.find(address);
                    // if(it != g_storage.end()){
                    //     //
                    //     g_storage[address]=data;
                    //     printf("updating address=0x%llx with data=0x%llx\n", address, data);
                    // } else {
                    //     g_storage.insert(std::pair<long long, long long>(address,data));
                    //     printf("writting at address=0x%llx  data=0x%llx\n", address, data);
                    // }
                    
                }
                break;
            case 14: //read
                {   

                    read_idx = addr_to_index((uint32_t)address);
                    if(read_idx < MAX_IDX){
                        printf("read will be performed from initialised address=0x%llx\n", address);
                        // tmp_read_addr = elf_data + idx;
                    } else {
                        printf("Error: read would be performed from un-initialised address=0x%llx which is out of memory\n", address);
                        // printf("read will be performed from un-initialised address=0x%llx\n", address);
                    }
                    // g_read_it = g_storage.find(address);
                    // if(g_read_it != g_storage.end()){
                    //     printf("read will be performed from initialised address=0x%llx\n", address);
                    // } else {
                    //     printf("read will be performed from un-initialised address=0x%llx\n", address);
                    // }
                        
                }
                break;
            };

        return 1;
    }
