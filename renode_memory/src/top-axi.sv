// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51



`include "renode_assign.svh"

// verilog_lint: waive-start package-filename
import renode_memory_pkg::*;

module master (
    input  logic                             clk,
    input  logic                             areset_n,
    input  renode_pkg::bus_connection        bus_controller,
    output renode_memory_pkg::mem_out_req_t  mem_out_req_o,
    input  renode_memory_pkg::mem_out_resp_t mem_out_req_i
);

  renode_axi_if m_axi_if (.aclk(clk));
  assign m_axi_if.areset_n = areset_n;

  `__RENODE_TO_REQ(mem_out_req_o, m_axi_if)
  `__RESP_TO_RENODE(m_axi_if, mem_out_req_i)


  renode_axi_manager m_axi_mem (
      m_axi_if,
      bus_controller
  );

  address_t address = 32'h10;
  valid_bits_e data_bits = renode_pkg::Word;
  data_t wdata = 32'h100;
  data_t rdata = 32'h101;
  ;
  bit is_error;


  task stopAtError(input bit is_error, input string file, input int line);
    begin
      if (is_error) begin
        $display("\n Error at  %s:%0d\n", file, line);
        $finish(1);
      end
    end
  endtask

  always_ff @(posedge m_axi_if.aclk) begin

    //negative test
    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.write(address, data_bits, wdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    if (wdata != rdata) begin
      string error_msg;
      error_msg = $sformatf("Test for Word data has FAILED! wdata!= rdata\n");
      $error(error_msg);
      $finish(1);
    end
    // QuadWord test 
    address = 32'h1000;
    data_bits = renode_pkg::QuadWord;
    wdata = 32'h200;
    rdata = 32'h201;

    //negative test
    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.write(address, data_bits, wdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    
    if (wdata != rdata) begin
      string error_msg;
      error_msg = $sformatf("Test for QuadWord data has FAILED! wdata!= rdata\n");
      $error(error_msg);
      $finish(1);
    end else begin
      $display("\nTest OK. Ending simulation with code 0.");
      $finish(0);
    end
  end

endmodule



module top (
    input logic clk_i,
    input logic rst_ni
);


  renode_connection connection = new();
  bus_connection    bus_peripheral = new(connection);
  bus_connection    bus_controller = new(connection);
  renode_axi_if axi_if (.aclk(clk_i));
  assign axi_if.areset_n = rst_ni;
  renode_memory_pkg::axi_connection_req_t  axi_req;
  renode_memory_pkg::axi_connection_resp_t axi_resp;
  //


  `__RENODE_TO_RESP(axi_resp, axi_if)
  `__REQ_TO_RENODE(axi_if, axi_req)

  renode_memory mem (
      .s_axi_if(axi_if),
      .bus_peripheral(bus_peripheral)
  );

  master ctr (
      .clk(clk_i),
      .areset_n(rst_ni),
      .bus_controller(bus_controller),
      .mem_out_req_o(axi_req),
      .mem_out_req_i(axi_resp)
  );
  // Print some stuff as an example
  initial begin
    if ($test$plusargs("trace") != 0) begin
      $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
      $dumpfile("logs/vlt_dump.vcd");
      $dumpvars();
    end

    $display("[%0t] Model running...\n", $time);
  end

  always_ff @(posedge clk_i) begin
    if (!rst_ni) begin
      bus_peripheral.reset_assert();
    end
  end

  always @(bus_peripheral.reset_assert_response) begin
    bus_controller.reset_assert();
  end

  always @(bus_controller.reset_assert_response) begin
    bus_controller.reset_deassert();
  end


endmodule
