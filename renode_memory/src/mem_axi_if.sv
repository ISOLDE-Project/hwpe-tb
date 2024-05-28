//
// Copyright (c) 2010-2024 Antmicro
//
// This file is licensed under the MIT License.
// Full license text is available in 'licenses/MIT.txt'.
//

interface mem_axi_if #(
    parameter int unsigned AddrWidth = 64,
    parameter int unsigned DataWidth = 64,
    parameter int unsigned IdWidth   = 8,
    parameter int unsigned UserWidth = 1
) (
    input logic clk_i,
    input logic rst_ni
);

  typedef logic [AddrWidth-1:0] addr_t;
  typedef logic [DataWidth-1:0] data_t;
  typedef logic [DataWidth/8-1:0] strb_t;
  typedef logic [IdWidth-1:0] id_t;
  typedef logic [UserWidth-1:0] user_t;

  `AXI_TYPEDEF_ALL(bus, addr_t, id_t, data_t, strb_t, user_t)
  bus_req_t  req;
  bus_resp_t resp;

endinterface
