`timescale 1ns / 100ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/14/2023 08:41:39 AM
// Design Name: 
// Module Name: latch
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


module latch (
    input bit ap_clk,
    input bit ap_rst_n,
    input bit latch,
    input bit out_ack,
    input logic [31:0] data_in,
    output bit out_vld,
    output logic [31:0] data_out
);
  // Internal register to store the latched data
  // Internal register to store the delayed data
  logic [31:0] reg_data;
  bit          reg_out_vld;
  always @(posedge ap_clk) begin
    if (!ap_rst_n) begin
      reg_data = 0;
      reg_out_vld = 0;
    end else begin
      if (reg_out_vld) begin
        if (out_ack) begin
          reg_out_vld = 0;
        end
      end else begin
        if (latch) begin
          reg_data = data_in;
          reg_out_vld = 1;
        end
      end
    end
  end
  assign data_out = reg_data;
  assign out_vld  = reg_out_vld;
endmodule
