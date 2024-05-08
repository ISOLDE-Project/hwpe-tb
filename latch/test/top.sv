`timescale 1ns /100ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/14/2023 09:38:57 AM
// Design Name: 
// Module Name: testbench
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module top(

    input  bit ap_clk,
    input  bit ap_rst_n,
    input  bit latch,
    input  bit out_ack,
    input  logic [31:0] data_in,
    output logic [31:0] data_out,
    output bit out_vld
); 

    latch DUT(
        .ap_clk(ap_clk),
        .ap_rst_n(ap_rst_n),
        .latch(latch),
        .out_ack(out_ack),
        .data_in(data_in),
        .out_vld(out_vld),
        .data_out(data_out)
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
endmodule
