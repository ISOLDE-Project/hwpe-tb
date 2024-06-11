#include "Vtop.h"
#include "Vtop__Dpi.h"
#include "lib/elfLoader.hpp"
#include <cstdint>
#include <map>

typedef long long addr_type;
typedef long long data_type;

static constexpr uint32_t ROM_RN_ORIGIN = 0; // 0x00001000;
// extern uint32_t *elf_data;
// extern unsigned MAX_IDX;
extern ELFLoader loader;
unsigned read_idx;

static inline uint32_t addr_to_index(const uint32_t addr) {
  uint32_t tmp_sh_addr = addr - ROM_RN_ORIGIN;
  return static_cast<uint32_t>(tmp_sh_addr >> 2);
}


  static bool fetchData(const addr_type addr_, DataUnion &dst) {
    return loader.fetchData(addr_, dst.ui32);
  }
  static bool pushData(const addr_type addr_, DataUnion &src) {
    return loader.pushData(addr_, src.ui32);
  }


struct RX_data {
  RX_data() : action(0), address(0), data(0) {}
  int action;
  addr_type address;
  data_type data;
};

// storage_type g_storage;
// storage_type::iterator g_read_it;
union DataUnion64 {
  uint64_t ui64;
  uint32_t ui32[2];
};
//
RX_data g_rx;

static const char *decodeAction(int action) {
  switch (action) {
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



// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:39:32
extern void renodeDPIConnect(int receiverPort, int senderPort,
                             const char *address) {
  printf("\nrenodeDPIConnect\n");
}
// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:45:32
extern void renodeDPIDisconnect() { printf("\renodeDPIDisconnect\n"); }
// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:47:31
extern svBit renodeDPIIsConnected() {
  printf("\renodeDPIIsConnected\n");
  return 1;
}
// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:49:32
extern void renodeDPILog(int logLevel, const char *data) {
  printf("\renodeDPILog:%s\n", data);
}
// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:54:31
extern svBit renodeDPIReceive(int *action, long long *address,
                              long long *data) {
  // copy the "received data"
  *action = g_rx.action;
  *address = g_rx.address;
  *data = g_rx.data;
  DataUnion src;
  bool fetchStatus = fetchData(g_rx.address, src);
  switch (g_rx.action) {
  default:
    printf("renodeDPISendToAsync, unknown action=%d, for address=0x%llx, "
           "data=0x%llx\n",
           *action, *address, *data);
    return 0;
  case 12: // getDoubleWord = 12,
    *data = static_cast<data_type>(src.ui32);
    break;
  case 14: // getWord = 14,
  {
    int slot = *address % 4 ? 1 : 0;
    *data = static_cast<data_type>(src.ui16[slot]);
  } break;
  case 30: // getQuadWord = 30,
  {
    DataUnion src;
    DataUnion64 dst;
    fetchData(g_rx.address, src);
    dst.ui32[0] = src.ui32;
    fetchData(g_rx.address + 4, src);
    dst.ui32[1] = src.ui32;
    *data = dst.ui64;
  } break;
  };

  printf("renodeDPIReceive: status= %s, value =0x%llx  @address=0x%llx\n",
         fetchStatus ? "OK" : "FAILED", *data, *address);
  return 1;
}

// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:60:31
extern svBit renodeDPISend(int action, long long address, long long data) {
  printf("\renodeDPISend\n");
  return 1;
}
// DPI import at
// /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:66:31
extern svBit renodeDPISendToAsync(int action, long long address,
                                  long long data) {
  // simulate sending and reception
  g_rx.action = 0;
  switch (action) {
  default:
    printf("renodeDPISendToAsync, unknown action=%d, for address=0x%llx, "
           "data=0x%llx\n",
           action, address, data);
    return 0;
  case 11: // pushDoubleWord = 11,
  {
    DataUnion src;
    src.ui32 = static_cast<uint32_t>(data);
    pushData(address, src);
  } break;
  case 13: // pushWord = 13,
  {
    DataUnion src;
    fetchData(address, src);
    int slot = address % 4 ? 1 : 0;
    src.ui16[slot] = static_cast<uint16_t>(data);
    pushData(address, src);
  } break;
  case 29: // pushQuadWord = 29,
  {

    DataUnion dst;
    DataUnion64 src;
    src.ui64 = data;
    dst.ui32 = src.ui32[0];
    pushData(address, dst);
    dst.ui32 = src.ui32[1];
    pushData(address + 4, dst);

  } break;
    ////////////////////////////////////////////////////
  case 12: // getDoubleWord = 12,
  case 14: // getWord = 14,
  case 30: // getQuadWord = 30,
  {
    g_rx.action = action;
    g_rx.address = address;
    g_rx.data = data;

  } break;
  }

  return 1;
}



// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:39:32
// extern void renodeDPIConnect(int receiverPort, int senderPort,
//                              const char *address) {
//   printf("\nrenodeDPIConnect\n");
// }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:45:32
// extern void renodeDPIDisconnect() { printf("\renodeDPIDisconnect\n"); }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:47:31
// extern svBit renodeDPIIsConnected() {
//   printf("\renodeDPIIsConnected\n");
//   return 1;
// }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:49:32
// extern void renodeDPILog(int logLevel, const char *data) {
//   printf("\renodeDPILog:%s\n", data);
// }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:54:31
// extern svBit renodeDPIReceive(int *action, long long *address,
//                               long long *data) {
//   printf("\renodeDPIReceive\n");
//   if (read_idx < loader.max_idx()) {
//     // printf("read will be performed from initialised address=0x%x\n",
//     // *address);
//     *data = *(loader.getStorage() + read_idx);
//   } else {
//     printf("read is performed from unknown address=0x%llx\n", *address);
//     *data = static_cast<uint32_t>(0);
//   }
//   return 1;
// }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:60:31
// extern svBit renodeDPISend(int action, long long address, long long data) {
//   printf("\renodeDPISend\n");
//   return 1;
// }
// // DPI import at
// // /home/uic52463/hdd2/isolde-project/hwpe-tb/renode_memory/hdl/imports/renode_pkg.sv:66:31
// extern svBit renodeDPISendToAsync(int action, long long address,
//                                   long long data) {
//   printf("\renodeDPISendToAsync, action=%d,address=0x%llx, data=0x%llx\n",
//          action, address, data);
//   switch (action) {
//   default:
//     printf("\renodeDPISendToAsync, unknowned action=%d, for address=0x%llx, "
//            "data=0x%llx\n",
//            action, address, data);
//     break;
//   case 13: // write
//   {
//     unsigned idx = addr_to_index((uint32_t)address);
//     if (idx < loader.max_idx()) {
//       uint32_t *tmp_addr = loader.getStorage() + idx;
//       *tmp_addr = (uint32_t)data;
//       printf("updating address=0x%llx with data=0x%llx\n", address, data);
//     } else {

//       printf("writting at address=0x%llx  data=0x%llx\n", address, data);
//     }
//   } break;
//   case 14: // read
//   {

//     read_idx = addr_to_index((uint32_t)address);
//     if (read_idx < loader.max_idx()) {
//       printf("read will be performed from initialised address=0x%llx\n",
//              address);
//     } else {
//       printf("Error: read would be performed from un-initialised "
//              "address=0x%llx which is out of memory\n",
//              address);
//     }
//   } break;
//   };

//   return 1;
// }
