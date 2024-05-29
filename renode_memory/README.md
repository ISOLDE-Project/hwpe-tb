# renode_memory

folder hdl is copied from [renode github](https://github.com/renode/renode/tree/master/src/Plugins/VerilatorPlugin/VerilatorIntegrationLibrary/hdl)

## Simulation

make sure that verilator is installed locally, see [../README.md](../README.md)

```
 make run
```

### Errors
A possible sollution is to start fresh again:  
```
make clean
make run
```

### Clear bender local files
```
make hard-clean
make run
```

### Run memory read test
```
make clean
make run TOP_MODULE_SV=src/top-axi-memread.sv
```
Expected output:
```
Test completed

- src/top-axi-memread.sv:128: Verilog $finish
```