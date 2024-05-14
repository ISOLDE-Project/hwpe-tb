// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

package renode_memory_pkg;

`include "axi/typedef.svh"

  localparam int unsigned AddrWidth = 32;
  localparam int unsigned DataWidth = 32;
  localparam int unsigned IdWidthIn = 8;
  localparam int unsigned UserWidth = 1;


  typedef logic [AddrWidth-1:0] addr_t;
  typedef logic [DataWidth-1:0] data_t;
  typedef logic [DataWidth/8-1:0] strb_t;
  typedef logic [IdWidthIn-1:0] id_t;
  typedef logic [UserWidth-1:0] user_t;



   `AXI_TYPEDEF_ALL(mem_in, addr_t, id_t, data_t, strb_t, user_t)
   `AXI_TYPEDEF_ALL(mem_out, addr_t, id_t, data_t, strb_t, user_t)

   `AXI_TYPEDEF_ALL(axi_connection, addr_t, id_t, data_t, strb_t, user_t)


endpackage