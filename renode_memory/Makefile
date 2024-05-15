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
#VERILATOR_FLAGS += -Wall
# Make waveforms
VERILATOR_FLAGS += --trace
# Check SystemVerilog assertions
VERILATOR_FLAGS += --assert
# Generate coverage analysis
#VERILATOR_FLAGS += --coverage
# Run Verilator in debug mode
#VERILATOR_FLAGS += --debug
# Add this trace to get a backtrace in gdb
#VERILATOR_FLAGS += --gdbbt
#Time 
VERILATOR_FLAGS += --timing
#Warning This indicates that a blocking assignment (=) is used in a sequential block
VERILATOR_FLAGS += -Wno-BLKSEQ
#Warning USEDSIGNAL Warns that the specified signal is never used/consume
#VERILATOR_FLAGS += -Wno-UNUSEDSIGNAL
#
VERILATOR_FLAGS += -Wno-TIMESCALEMOD
#
VERILATOR_FLAGS += -Wno-PINMISSING
#
VERILATOR_FLAGS += -Wno-WIDTHEXPAND
#
VERILATOR_FLAGS += -Wno-WIDTHTRUNC
#
VERILATOR_FLAGS += -Wno-REDEFMACRO
#
VERILATOR_FLAGS += -Wno-ASSIGNIN

HDL_DIR := hdl
SV_FILES := $(wildcard $(SRC_DIR)/*.sv)
TOP_MODULE := top

# CPPFLAGS += -Icc/lib
LIBDIR += -Icc/lib
# LIB = $(wildcard $(LIBDIR)/%.h)
all: files
	$(VERILATOR) $(VERILATOR_FLAGS) -I$(ROOT_DIR)/renode_memory/hdl/imports -CFLAGS -Icc/lib -LDFLAGS -Icc/lib -f files src/top.sv  cc/sim_main.cc cc/renode_dpi.cc cc/lib/elfFile.cpp --top-module $(TOP_MODULE)

files: $(SV_FILES) Bender.yml
	bender script verilator -t rtl -t verilator > $@

run: obj_dir/V$(TOP_MODULE)
	obj_dir/V$(TOP_MODULE)

obj_dir/V$(TOP_MODULE): obj_dir/V$(TOP_MODULE).mk
	$(MAKE) -C obj_dir/ -f V$(TOP_MODULE).mk -j6

.PHONY: obj_dir/V$(TOP_MODULE).mk

obj_dir/V$(TOP_MODULE).mk : all

show-config:
	$(VERILATOR) -V
	@echo "Verilator usage:"
	@echo "$(VERILATOR) $(VERILATOR_FLAGS)"



.PHONY: clean
clean:
	rm -f files
	rm -fr obj_dir
	rm -fr logs

.PHONY: hard-clean
hard-clean:
	rm -f files
	rm -fr obj_dir
	rm -fr logs
	rm -fr .bender
	rm -fr Bender.lock
