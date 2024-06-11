// DESCRIPTION: Verilator: Verilog example module
//
// This file ONLY is placed under the Creative Commons Public Domain, for
// any use, without warranty, 2017 by Wilson Snyder.
// SPDX-License-Identifier: CC0-1.0
//======================================================================

// For std::unique_ptr
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
// Include common routines
#include <verilated.h>

// Include model header, generated from Verilating "top.v"
#include "Vtop.h"
#include "lib/CLI11.hpp"
#include "lib/elfLoader.hpp"
#include "lib/cnpy.h"

#include <verilated_vcd_c.h>

#define CYCLES 1000
#define CLK_DELAY 200
#define TIMEOUT 1 * CYCLES

// Current simulation time (64-bit unsigned)
vluint64_t main_time = 0;

// uint32_t *elf_data;
ELFLoader loader;

unsigned MAX_IDX;
// Legacy function required only so linking works on Cygwin and MSVC++
double sc_time_stamp() { return 0; }

int main(int argc, char **argv) {

  std::string binaryFile; // assign to default
  std::string outputFile ("empty");
  std::vector<std::string> verilatorArgs;
  std::vector<char *> verArgs;
  verArgs.reserve(verilatorArgs.size() + 1);
  verArgs.push_back(argv[0]);

  CLI::App app{"ELF loader"};
  app.add_option("-f,--file", binaryFile,
                 "Specifies the RISC-V program binary file (elf)")
      ->required();
  app.add_option("-o,--output", outputFile, "Specifies the output file");
  app.add_option("-v,--verilator", verilatorArgs, "Specifies verilator args");
  CLI11_PARSE(app, argc, argv);
  for (auto s : verilatorArgs) {
    std::string tmp = s;
    if(tmp.find("+") != std::string::npos){
        tmp = tmp.replace(tmp.find("+"), 1, "-");
    }
    verArgs.push_back(const_cast<char *>(tmp.c_str()));
  }

  loader.readElf(binaryFile);
//   elf_data = loader.getStorage();
//   MAX_IDX = loader.max_idx();
  // This is a more complicated example, please also see the simpler
  // examples/make_hello_c.

  // Create logs/ directory in case we have traces to put under it
  Verilated::mkdir("logs");

  // Construct a VerilatedContext to hold simulation time, etc.
  // Multiple modules (made later below with Vtop) may share the same
  // context to share time, or modules may have different contexts if
  // they should be independent from each other.

  // Using unique_ptr is similar to
  // "VerilatedContext* contextp = new VerilatedContext" then deleting at end.
  const std::unique_ptr<VerilatedContext> contextp{new VerilatedContext};
  // Do not instead make Vtop as a file-scope static variable, as the
  // "C++ static initialization order fiasco" may cause a crash

  // Set debug level, 0 is off, 9 is highest presently used
  // May be overridden by commandArgs argument parsing
  contextp->debug(0);

  // Randomization reset policy
  // May be overridden by commandArgs argument parsing
  contextp->randReset(2);

  // Verilator must compute traced signals
  contextp->traceEverOn(true);

  // Pass arguments so Verilated code can see them, e.g. $value$plusargs
  // This needs to be called before you create any model
  uint8_t nrArg = 2;
  if (outputFile.compare("empty") != 0){ // return 0 if equal
    nrArg += 2;
  }
  if (!verArgs.empty()){
    nrArg++;
  }
  contextp->commandArgs(argc - nrArg, verArgs.data());

  // Construct the Verilated model, from Vtop.h generated from Verilating
  // "top.v". Using unique_ptr is similar to "Vtop* top = new Vtop" then
  // deleting at end. "TOP" will be the hierarchical name of the module.
  const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

  VerilatedVcdC *tfp = new VerilatedVcdC;
  top->trace(tfp, 99); // Trace 99 levels of hierarchy
  Verilated::mkdir("logs");
  tfp->open("logs/sim.vcd");

  bool endOfTestSequence = false;
  top->clk_i = 0;
  top->rst_ni = 0;
  contextp->time(0);
  int hold_reset = 3;
  // Simulate until $finish
  while (!contextp->gotFinish() && main_time < TIMEOUT && !endOfTestSequence) {
    // Historical note, before Verilator 4.200 Verilated::gotFinish()
    // was used above in place of contextp->gotFinish().
    // Most of the contextp-> calls can use Verilated:: calls instead;
    // the Verilated:: versions just assume there's a single context
    // being used (per thread).  It's faster and clearer to use the
    // newer contextp-> versions.

    contextp->timeInc(1); // 1 timeprecision period passes...
    // Historical note, before Verilator 4.200 a sc_time_stamp()
    // function was required instead of using timeInc.  Once timeInc()
    // is called (with non-zero), the Verilated libraries assume the
    // new API, and sc_time_stamp() will no longer work.

    if (contextp->time() % 5 == 0) {
      top->clk_i = !top->clk_i;
      if (hold_reset)
        hold_reset--;
    }

    if (!hold_reset) {
      top->rst_ni = 1; // Deassert rst_ni
    }

    // Evaluate model
    // (If you have multiple models being simulated in the same
    // timestep then instead of eval(), call eval_step() on each, then
    // eval_end_step() on each. See the manual.)
    top->eval();
    tfp->dump(main_time);
    main_time++;
  }

  // Final model cleanup
  top->final();
  tfp->close();
  printf("main_time=%fns\n",
         ((double)main_time) / (-contextp->timeprecision()));

  // Coverage analysis (calling write only after the test is known to pass)
#if VM_COVERAGE
  Verilated::mkdir("logs");
  contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Final simulation summary
    contextp->statsPrintSummary();

    if (outputFile.compare("empty") != 0){ // return 0 if equal
        std::string name;
        int index = outputFile.rfind(".");
        if(index != std::string::npos){
            name = outputFile.substr(0, index);
        } else {
            name = outputFile;
            outputFile += std::string(".npy");
        }

        cnpy::npy_save(outputFile,&loader.storage[0], {loader.storage.size()}, "w");
    }
    // Return good completion status
    // Don't use exit() or destructor won't get called
    return 0;

}
