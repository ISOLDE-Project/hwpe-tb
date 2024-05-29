/* This file is used to generate link.ld, Ara's linker script,
   which depends on the number of lanes of the current configuration */

OUTPUT_ARCH("riscv")
ENTRY(_start)

MEMORY {
  ROM :    ORIGIN = 0x00001000, LENGTH = 0X0c0
  STACK  : ORIGIN = 0x000010c0, LENGTH = 0x100
  RAM  :   ORIGIN = 0x000011c0, LENGTH = 0x100 
}


SECTIONS {

   __stack_size =0x100;
   

  .text : ALIGN(8){
    *(.text.init)
    *(.text)
    *(.text.*)
  } > ROM

 .rodata  : ALIGN(8) { 
    *(.rodata .rodata.* .gnu.linkonce.r.*) }
   > ROM

    .stack  ALIGN(8):  {
        PROVIDE(metal_segment_stack_begin = .);
         
         . += __stack_size; 
         PROVIDE( __sp$ = . );
        PROVIDE(metal_segment_stack_end = .);
    } > STACK

  .data :  {
    *(.data)
    *(.data.*)
    PROVIDE(__input = .);
  } > RAM

  .sdata   :  {
    __global_pointer$ = . + 0x800;
    *(.srodata.cst8) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata .srodata.*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } > RAM

   .bss : ALIGN(8) {
    __bss_start = .;
    *(.bss)
    *(.sbss .sbss.* .sbss2 .sbss2.* .gnu.linkonce.sb2.*);
    __bss_end = .;
  } > RAM
    
    PROVIDE( metal_segment_bss_source_start = LOADADDR(.tbss) );
    PROVIDE( metal_segment_bss_target_start = ADDR(.bss) );
    PROVIDE( metal_segment_bss_target_end = ADDR(.bss) + SIZEOF(.bss) );
}
