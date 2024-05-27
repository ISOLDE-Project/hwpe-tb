#include "MemorySim.hh"




namespace ISOLDE{

 ELFLoader MemorySim::loader;


 
MemorySim::MemorySim(int argc, char** argv): contextp{new VerilatedContext},main_time (0) {

    // Create logs/ directory in case we have traces to put under it
    Verilated::mkdir("logs");
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
    contextp->commandArgs(argc, argv);

}

int MemorySim::main(std::string& binaryFile, uint32_t timeOut){

    loader.readElf(binaryFile);
       fprintf(stderr,"PROGRAM_SIZE=%u Bytes, %.2f MB \n",
                                  loader.max_idx(),
                                  (float)(loader.max_idx()/(1024.*1024.)));
     const std::unique_ptr<Vtop> top{new Vtop{contextp.get(), "TOP"}};

    //VerilatedVcdC *tfp = new VerilatedVcdC;
    auto tfp = std::make_unique<VerilatedVcdC>();
    top->trace(tfp.get(), 99);  // Trace 99 levels of hierarchy
    Verilated::mkdir("logs");
    tfp->open("logs/sim.vcd");

    bool endOfTestSequence=false;
    top->clk_i=0;
    top->rst_ni=0;
    contextp->time(0);
    int hold_reset=3;
    // Simulate until $finish
    while (!contextp->gotFinish()  && main_time < timeOut && !endOfTestSequence ) {
        // Historical note, before Verilator 4.200 Verilated::gotFinish()
        // was used above in place of contextp->gotFinish().
        // Most of the contextp-> calls can use Verilated:: calls instead;
        // the Verilated:: versions just assume there's a single context
        // being used (per thread).  It's faster and clearer to use the
        // newer contextp-> versions.

                 contextp->timeInc(1);  // 1 timeprecision period passes...
        // Historical note, before Verilator 4.200 a sc_time_stamp()
        // function was required instead of using timeInc.  Once timeInc()
        // is called (with non-zero), the Verilated libraries assume the
        // new API, and sc_time_stamp() will no longer work.


        if (contextp->time() % 5 == 0){
            top->clk_i=!top->clk_i;
            if(hold_reset) hold_reset--;
        }    

        if (!hold_reset) {                
                top->rst_ni = 1;  // Deassert rst_ni
                
            }
       

        // Evaluate model
        // (If you have multiple models being simulated in the same
        // timestep then instead of eval(), call eval_step() on each, then
        // eval_end_step() on each. See the manual.)
        top->eval();
        tfp->dump(main_time);
         main_time ++;

    }

    // Final model cleanup
    top->final();
    tfp->close();
    printf("main_time=%fns\n", ((double) main_time)/(-contextp->timeprecision()) );

    // Coverage analysis (calling write only after the test is known to pass)
#if VM_COVERAGE
    Verilated::mkdir("logs");
    contextp->coveragep()->write("logs/coverage.dat");
#endif

    // Final simulation summary
    contextp->statsPrintSummary();

    // Return good completion status
    // Don't use exit() or destructor won't get called
    return 0;
}

}//namespace ISOLDE