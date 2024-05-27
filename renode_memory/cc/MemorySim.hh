// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

#pragma once
#include <string>
// For std::unique_ptr
#include <memory>

// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"

#include <verilated_vcd_c.h>

#include "elfLoader.hpp"

namespace ISOLDE{
    union DataUnion{
  uint32_t ui32;
  uint16_t ui16[2];
  uint8_t ui8[4];
};

struct MemorySim{

typedef ELFLoader::addr_type addr_type;

    MemorySim(int argc, char** argv);
    int main(std::string& binaryfile, uint32_t);
   
    //
    const std::unique_ptr<VerilatedContext> contextp;
    vluint64_t main_time;
    //static ELFLoader& getELFLoader();
 
    static void fetchData(const addr_type addr_, DataUnion& dst)  {
        volatile uint32_t* data = loader.getStorage();
         auto  idx= ELFLoader::addr_to_index(addr_);
        dst.ui32 = data[idx];
  }
    static void pushData(const addr_type addr_,  DataUnion& src)  {
  volatile uint32_t* data = loader.getStorage();
         auto  idx= ELFLoader::addr_to_index(addr_);
    data[idx]=src.ui32;
  }

    private:
    static ELFLoader loader;
};
}//namespace ISOLDE