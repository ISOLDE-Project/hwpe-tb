// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

import renode_pkg::renode_connection, renode_pkg::bus_connection;
import renode_pkg::message_t, renode_pkg::address_t, renode_pkg::data_t, renode_pkg::valid_bits_e;



module master (
          renode_axi_if              m_axi_if,
    input renode_pkg::bus_connection bus_controller

);


  renode_axi_manager m_axi_mem (
      m_axi_if,
      bus_controller
  );


  address_t address = 32'h10;
  valid_bits_e data_bits = renode_pkg::Word;
  data_t wdata = 32'h100;
  data_t rdata = 32'h101;

  bit is_error;


  always_ff @(posedge m_axi_if.aclk) begin

    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.write(address, data_bits, wdata, is_error);
    repeat (8) @(posedge m_axi_if.aclk);
    bus_controller.read(address, data_bits, rdata, is_error);
    if (wdata != rdata) begin
      string error_msg;
      error_msg = $sformatf("Error! wdata!= rdata\n");
      $error(error_msg);
      $finish;
    end
  end

endmodule




module top (
    input logic clk,
    input logic reset
);


  renode_connection connection = new();
  bus_connection    bus_peripheral = new(connection);
  bus_connection    bus_controller = new(connection);
  time              renode_time = 0;

  renode_axi_if axi_if (.aclk(clk));

  renode_memory mem (
      .s_axi_if(axi_if),
      .bus_peripheral(bus_peripheral)
  );

  master ctr (
      .m_axi_if(axi_if),
      .bus_controller(bus_controller)
  );
  // Print some stuff as an example
  initial begin
    if ($test$plusargs("trace") != 0) begin
      $display("[%0t] Tracing to logs/vlt_dump.vcd...\n", $time);
      $dumpfile("logs/vlt_dump.vcd");
      $dumpvars();
    end
    axi_if.areset_n = 0;
    $display("[%0t] Model running...\n", $time);
  end

  always_ff @(posedge clk) begin
    if (!reset) begin
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
