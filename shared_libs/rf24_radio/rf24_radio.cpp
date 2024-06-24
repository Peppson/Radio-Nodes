
#include "rf24_radio.h"
#include "hardware.h"
#include "utility.h"


void RF24Radio::begin() {
    if (!RF24::begin() && !RF24::isChipConnected()) {
        log("RF24 not found\n");
        return;
    }

    // Addresses: Master, this device
    constexpr byte _pipe_address[2][3] = { 
        {0x1, 0x2, (byte)RF24_MASTER_ADDRESS}, 
        {0x1, 0x2, (byte)NODE_ID} 
    };

    // Setup
    setPALevel(RF24_OUTPUT);                        // Transmitter strength   
    setChannel(RF24_CHANNEL);                       // Channel 
    setDataRate(RF24_DATARATE);                     // Datarate (RF24_1MBPS, RF24_2MBPS, RF24_250KBPS)
    setAddressWidth(3);                             // Address size in bytes
    setRetries(5, 15);                              // Acknowledgement package, retry up to 15 times.
    setPayloadSize(8);                              // Payload size 8 Bytes (RF24Radio::EncodedMessage)
    setCRCLength(rf24_crclength_e::RF24_CRC_8);     // CRC size 1 Byte

    // Begin 
    openWritingPipe(_pipe_address[0]);              // Send to which address, TX
    openReadingPipe(RF24_PIPE, _pipe_address[1]);   // Listen on which address, RX
    log("RF24 On!\n");
    delay(250);
}


void RF24Radio::power_up() {
    powerUp();
    startListening();
    delay(5);
}


void RF24Radio::power_down() {
    stopListening();
    powerDown();
    delay(5);
}


bool RF24Radio::send_message(bool state, uint16_t data_0, uint16_t data_1, uint16_t data_2) {
    stopListening();
    WDT_FEED();
    
    // Temp struct for transmitting
    EncodedMessage msg = {0};

    // Populate msg before sending
    msg.combined    |= (RF24_THIS_ADDRESS & 0x7F);              // Set the first 7 bits to RF24_THIS_ADDRESS
    msg.combined    |= ((uint16_t) RF24_MASTER_ADDRESS << 7);   // Set the next 8 bits to RF24_MASTER_ADDRESS
    msg.combined    |= ((uint16_t) state << 15);                // Set the last bit to boolean
    msg.data_0      = data_0;                                   // Data 0
    msg.data_1      = data_1;                                   // Data 1
    msg.data_2      = data_2;                                   // Data 2

    // Send
    bool message_sent = false;
    for (size_t i = 0; i < 4; i++) {
        if (write(&msg, sizeof(msg))) {
            message_sent = true;
            break;
        } 
        Hardware::lightsleep(750);
    }

    startListening();
    return message_sent;
}


bool RF24Radio::wait_for_message(const uint16_t timeout_ms) {
    uint32_t end_time = millis() + timeout_ms;

    // Start listening
    while (millis() < end_time) {
        if (available()) {
            return get_available_message(); // CRC is handle internally, won't get here if message is corrupted
        }
        WDT_FEED();
        delay(1);    
    }

    return false;
}


bool RF24Radio::get_available_message() {
    WDT_FEED();

    // Read message from nrf24l01+ buffer
    EncodedMessage msg;
    read(&msg, sizeof(msg));

    // Populate DecodedMessage struct
    message.from_who    = msg.combined & 0x7F;                              // Extract first 7 bits of combined
    message.to_who      = (msg.combined >> 7) & 0xFF;                       // Extract next 8 bits
    message.state       = static_cast<bool>((msg.combined >> 15) & 0x01);   // Last bit as bool
    message.data_0      = msg.data_0;                                       // Data 0
    message.data_1      = msg.data_1;                                       // Data 1 
    message.data_2      = msg.data_2;                                       // Data 2

    // Verify   
    if (message.from_who == RF24_MASTER_ADDRESS && message.to_who == RF24_THIS_ADDRESS) {
        delay(20); // Make sures the acknowledgement package was sent
        return true;
    }

    return false;
}


void RF24Radio::print_message(const DecodedMessage& data) {
    log("\nfrom_who:\t%i\n", data.from_who);
    log("to_who:\t\t%i\n", data.to_who);
    log("state:\t\t%d\n", data.state);
    log("data_0:\t\t"); util::print_int_hack(data.data_0);
    log("data_1:\t\t"); util::print_int_hack(data.data_1);
    log("data_2:\t\t"); util::print_int_hack(data.data_2);
    log("\n");
}


void RF24Radio::print_message(const EncodedMessage& data) {
    log("\nfrom_who:\t%i\n", data.combined & 0x7F);
    log("to_who:\t\t%i\n", (data.combined >> 7) & 0xFF);
    log("state:\t\t%d\n", static_cast<bool>((data.combined >> 15) & 0x01));
    log("data_0:\t\t"); util::print_int_hack(data.data_0);
    log("data_1:\t\t"); util::print_int_hack(data.data_1);
    log("data_2:\t\t"); util::print_int_hack(data.data_2);
    log("\n");
}


void RF24Radio::print_P_variant() {
    // Real NRF24L01+ variant or not. NRF24L01 (non +)
    log("NRF24L01+: %s\n", RF24::isPVariant() ? "true" : "false"); 
}
