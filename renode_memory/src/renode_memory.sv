// Copyleft
// Solderpad Hardware License, Version 0.51, see LICENSE for details.
// SPDX-License-Identifier: SHL-0.51

`include "renode_assign.svh"

module renode_memory #(
    int unsigned AddrWidth = 64,
    int unsigned DataWidth = 64,
    int unsigned IdWidth   = 8
) (

    input renode_pkg::bus_connection bus_peripheral,
    mem_axi_if axi_conn

);


  renode_axi_if #(
      .AddressWidth(AddrWidth),
      .DataWidth(DataWidth),
      .TransactionIdWidth(IdWidth)
  ) s_axi_if (
      .aclk(axi_conn.clk_i)
  );
  assign s_axi_if.areset_n = axi_conn.rst_ni;

  `__RENODE_TO_RESP(axi_conn.resp, s_axi_if)
  `__REQ_TO_RENODE(s_axi_if, axi_conn.req)

  renode_axi_subordinate s_axi_mem (
      s_axi_if,
      bus_peripheral
  );

  always @(bus_peripheral.read_transaction_request) begin
`ifdef RENODE_DEBUG
    $display("renode_memory.@read_transaction_request");
`endif
    read_transaction();
  end
  always @(bus_peripheral.write_transaction_request) begin
`ifdef RENODE_DEBUG
    $display("renode_memory.@:write_transaction_request");
`endif
    write_transaction();
  end



  task static read_transaction();
    message_t message;
`ifdef RENODE_DEBUG
    $display("renode_memory.read_transaction()");
`endif
    case (bus_peripheral.read_transaction_data_bits)
      renode_pkg::Byte: message.action = renode_pkg::getByte;
      renode_pkg::Word: message.action = renode_pkg::getWord;
      renode_pkg::DoubleWord: message.action = renode_pkg::getDoubleWord;
      renode_pkg::QuadWord: message.action = renode_pkg::getQuadWord;
      default: begin
        bus_peripheral.remote_connection.fatal_error($sformatf(
                                                     "Renode doesn't support access with the 'b%b mask from a bus controller.",
                                                     bus_peripheral.read_transaction_data_bits
                                                     ));
        bus_peripheral.read_respond(0, 1);
        return;
      end
    endcase
    message.address = bus_peripheral.read_transaction_address;
    message.data = 0;

    bus_peripheral.remote_connection.exclusive_receive.get();

    bus_peripheral.remote_connection.send_to_async_receiver(message);

    bus_peripheral.remote_connection.receive(message);

    bus_peripheral.remote_connection.exclusive_receive.put();
    bus_peripheral.read_respond(message.data, 0);
  endtask

  task static write_transaction();
    message_t message;

    case (bus_peripheral.write_transaction_data_bits)
      renode_pkg::Byte: message.action = renode_pkg::pushByte;
      renode_pkg::Word: message.action = renode_pkg::pushWord;
      renode_pkg::DoubleWord: message.action = renode_pkg::pushDoubleWord;
      renode_pkg::QuadWord: message.action = renode_pkg::pushQuadWord;
      default: begin
        bus_peripheral.remote_connection.fatal_error($sformatf(
                                                     "Renode doesn't support access with the 'b%b mask from a bus controller.",
                                                     bus_peripheral.read_transaction_data_bits
                                                     ));
        bus_peripheral.write_respond(1);
        return;
      end
    endcase
    message.address = bus_peripheral.write_transaction_address;
    message.data = bus_peripheral.write_transaction_data;

    bus_peripheral.remote_connection.send_to_async_receiver(message);
    bus_peripheral.write_respond(0);
  endtask

endmodule
