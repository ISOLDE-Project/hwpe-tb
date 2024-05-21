

XLEN?=32
APP=test

all: $(APP).riscv$(XLEN)


include ../../common/common.mk


$(APP).riscv$(XLEN): libsim.a  main.cpp.o
	@$(ECHO) +++ linking: $^
	$(LD)   $(RISCV_LDFLAGS)  -o $@  -Map $@.map $(LD_SCRIPT)  $^ 
	$(OBJDUMP) $(OBJDUMP_FLAGS) -D $@ > $@.dump
	objdump -h  $@ >$@.headers



