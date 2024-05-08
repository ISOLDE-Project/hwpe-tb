ROOT_DIR := $(shell git rev-parse --show-toplevel)

CXX := g++-10

num_cores := $(shell nproc)
num_cores_half := $(shell echo "$$(($(num_cores) / 2))")


INSTALL_PREFIX          ?= install
INSTALL_DIR             ?= $(ROOT_DIR)/${INSTALL_PREFIX}
GCC_INSTALL_DIR         ?= ${INSTALL_DIR}/riscv-gcc
LLVM_INSTALL_DIR        ?= ${INSTALL_DIR}/riscv-llvm
ISA_SIM_INSTALL_DIR     ?= ${INSTALL_DIR}/riscv-isa-sim
ISA_SIM_MOD_INSTALL_DIR ?= ${INSTALL_DIR}/riscv-isa-sim-mod
VERIL_INSTALL_DIR       ?= ${INSTALL_DIR}/verilator
VERIL_VERSION           ?= v5.024

VERILATOR = $(VERIL_INSTALL_DIR)/bin/verilator
VERILATOR_COVERAGE = $(VERIL_INSTALL_DIR)/bin/verilator_coverage


# Generate C++ in executable form
VERILATOR_FLAGS += -cc --exe
# Generate makefile dependencies (not shown as complicates the Makefile)
#VERILATOR_FLAGS += -MMD
# Optimize
VERILATOR_FLAGS += -x-assign fast
# Warn abount lint issues; may not want this on less solid designs
VERILATOR_FLAGS += -Wall
# Make waveforms
VERILATOR_FLAGS += --trace
# Check SystemVerilog assertions
VERILATOR_FLAGS += --assert
# Generate coverage analysis
VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt
#Time 
VERILATOR_FLAGS += --no-timing
#Warning This indicates that a blocking assignment (=) is used in a sequential block
VERILATOR_FLAGS += -Wno-BLKSEQ
#Warning USEDSIGNAL Warns that the specified signal is never used/consume
#VERILATOR_FLAGS += -Wno-UNUSEDSIGNAL


# Input files for Verilator
#VERILATOR_INPUT = -f input.vc top.v sim_main.cpp


all:
	$(VERILATOR) $(VERILATOR_FLAGS)  src/latch.sv test/top.sv test/sim_main.cpp --top-module top


show-config:
	$(VERILATOR) -V
	@echo "Verilator usage:"
	@echo "$(VERILATOR) $(VERILATOR_FLAGS)"

.PHONY: clean
clean:
	rm -fr obj_dir
