// DESCRIPTION: Verilator: Verilog example module
//
// This file ONLY is placed under the Creative Commons Public Domain, for
// any use, without warranty, 2017 by Wilson Snyder.
// SPDX-License-Identifier: CC0-1.0
//======================================================================

#include "CLI11.hpp"
#include "MemorySim.hh"
#include <stdint.h>

// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }

int main(int argc, char **argv) {

  std::string binaryFile;
  uint32_t timeOut{500};
  CLI::App app{"test bench"};
  app.add_option("-f,--file", binaryFile,
                 "Specifies the RISC-V program binary file (elf)")
      ->required();
  // app.add_option("-e,--export", export_py, "export binary file (elf) to py");
  CLI::Option *export_flag =
      app.add_flag("-e,--export", "export binary file (elf) to py");
  app.add_option("-t,--time-out", timeOut, "simulation timeout");
  CLI11_PARSE(app, argc, argv);

  if (export_flag->count()) {
    ELFLoader loader;
    loader.readElf(binaryFile);

    fprintf(stderr, "PROGRAM_SIZE=%u Bytes, %.2f KB \n", loader.max_idx(),
            (float)(loader.max_idx() / (1024.)));

    std::string name("code_image");
    FILE *pyFile =
        loader.openOrDefault(std::string("code_image.py"), "wb", NULL);
    fprintf(pyFile, "import numpy as np\n\n");
    py_pretty_print<uint32_t>(pyFile, name, loader);
    fprintf(pyFile, "\n\nstart=%u\n\n", loader.getStartAddress());
  }

  auto sim = std::make_unique<ISOLDE::MemorySim>(argc, argv);
  return sim->main(binaryFile, timeOut);
}