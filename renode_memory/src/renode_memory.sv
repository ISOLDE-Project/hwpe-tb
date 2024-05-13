module renode_memory (
    
    renode_axi_if s_axi_if,
    input renode_pkg::bus_connection bus_peripheral

);

  renode_axi_subordinate s_axi_mem (
      s_axi_if,
      bus_peripheral
  );

  always @(bus_peripheral.read_transaction_request) begin
    $display("slave::Event::read_transaction_request");
    read_transaction();
  end
  always @(bus_peripheral.write_transaction_request) begin
    $display("slave::Event::write_transaction_request");
    write_transaction();
  end



  task static read_transaction();
    message_t message;

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
