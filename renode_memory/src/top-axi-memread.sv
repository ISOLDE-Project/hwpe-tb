// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51



`include "renode_assign.svh"

// verilog_lint: waive-start package-filename
import renode_memory_pkg::*;
typedef int address32_t;
typedef int data32_t;

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

  address_t address_begin = 32'h1000;
  address_t address_end = 32'h11A8+32'h8; //elfloader adds 2 guard dwords at the end: 32'hcadebaba  and 32'hacdcface

  address_t address = address_begin;
  valid_bits_e data_bits = renode_pkg::DoubleWord;
  data_t rdata = 32'h101;
  bit is_error;
  bit test_done;

  //32 bit
  address32_t address32;
  data32_t rdata32;
  // File descriptor
  int fd;
  //offset for incrementing the memory
  integer offset;

  string format;

  task static py_pretty_print(input integer f, input address_t addr, input address_t addr_end,
                              data_t rdata);
    int rdata32;
    begin

     rdata32 = int'(rdata[31:0]);

      if (addr == addr_end) begin
        $fwrite(f, "0x%h", int'(rdata32));
      end else begin
        $fwrite(f, "0x%h,", rdata32);
      end
    end
  endtask

  task stopAtError(input bit is_error, input string file, input integer line);
    begin
      if (is_error) begin
        $display("\n Error at  %s:%0d\n", file, line);
        $finish(1);
      end
    end
  endtask


  task open_file(output int fd);
    begin
      fd = $fopen("mem_dump.py", "w");
      $fwrite(fd, "import numpy as np\n\n");
      $fwrite(fd, "%s = np.array([\n", "mem");
    end
  endtask

  task close_file(input int f1);
    begin
      if (f1 != 0) begin
        $fwrite(f1, "],dtype=np.uint32)\n");
        $fclose(f1);
      end
    end
  endtask

  task getOffset(input valid_bits_e data_bits, output integer offset);
    case (data_bits)
      renode_pkg::QuadWord: begin
        offset = 8;
      end
      renode_pkg::DoubleWord: begin
        offset = 4;
      end
      renode_pkg::Word: begin
        offset = 2;
      end

    endcase
  endtask

  initial begin
    open_file(fd);
    test_done = 0;
  end

  always_ff @(posedge m_axi_if.aclk) begin
   
    do @(posedge m_axi_if.aclk); while (!m_axi_if.areset_n);

    bus_controller.read(address, data_bits, rdata, is_error);
    stopAtError(is_error, `__FILE__, `__LINE__);
    py_pretty_print(fd, address, address_end, rdata);

    if (address == address_end) begin
      test_done <= 1;
    end else begin
      getOffset(data_bits, offset);
      address = address + offset;
    end

  end


  always_ff @(posedge m_axi_if.aclk) begin
    if (test_done) begin
      close_file(fd);
      $display("\n Test completed. Run make test to evaluate results\n");
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
