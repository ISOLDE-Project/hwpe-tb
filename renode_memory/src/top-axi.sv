// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51



`include "renode_assign.svh"

// verilog_lint: waive-start package-filename
import renode_memory_pkg::*;

module master (
    input  renode_pkg::bus_connection        bus_controller,
    mem_axi_if axi_conn
);

  renode_axi_if m_axi_if (.aclk(axi_conn.clk_i));
  assign m_axi_if.areset_n = axi_conn.rst_ni;

  `__RENODE_TO_REQ(axi_conn.req, m_axi_if)
  `__RESP_TO_RENODE(m_axi_if, axi_conn.resp)


  renode_axi_manager m_axi_mem (
      m_axi_if,
      bus_controller
  );

  address_t address = 32'h1018;
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

    do @(posedge m_axi_if.aclk); while (!m_axi_if.areset_n);
    
    $display("\n[%0t] Word test...\n", $time);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    address = address+2;
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    address = 32'h10c0;
    bus_controller.write(address, data_bits, wdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    if (wdata != rdata) begin
      string error_msg;
      error_msg = $sformatf("Test for Word data has FAILED! wdata!= rdata\n");
      $error(error_msg);
      $finish(1);
    end

    //DoubleWord test
    $display("\n[%0t] DoubleWord test...\n", $time);
    address = 32'h1000;
    data_bits = renode_pkg::DoubleWord;
    wdata = 32'h200;
    rdata = 32'h201;


    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    address = 32'h10c0;

    bus_controller.write(address, data_bits, wdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    
    // QuadWord test 
     $display("\n[%0t] QuadWord test...\n", $time);
    address = 32'h1000;
    data_bits = renode_pkg::QuadWord;
    wdata = 32'h400;
    rdata = 32'h401;

    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.write(address, data_bits, wdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
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

  mem_axi_if axi_conn(clk_i,rst_ni);

  renode_memory mem (
      .bus_peripheral(bus_peripheral),
      .axi_conn
  );

  master ctr (
      .bus_controller(bus_controller),
      .axi_conn
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
