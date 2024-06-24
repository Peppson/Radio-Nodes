
#pragma once
#include "../global_config.h"
#include <RF24.h>
#include "nRF24L01.h"
#include "SPI.h"


class RF24Radio : public RF24 {        
public:
    RF24Radio(uint8_t CE = PIN_CE, uint8_t CSN = PIN_CSN) : RF24(CE, CSN) {};
    ~RF24Radio() = default;

    // Used when sending
    struct EncodedMessage {
        uint16_t combined; // combined = from_who (7bits), to_who (8bits), state (1bit)
        uint16_t data_0;
        uint16_t data_1;
        uint16_t data_2;
    };

    // Used internally
    struct DecodedMessage {
        uint8_t from_who;
        uint8_t to_who;
        bool state;
        uint16_t data_0;
        uint16_t data_1;
        uint16_t data_2;
    } message;

    void begin();
    void power_up();
    void power_down();
    bool send_message(bool state, uint16_t data_0, uint16_t data_1, uint16_t data_2);
    bool wait_for_message(const uint16_t timeout_ms);
    bool get_available_message();
    void print_message(const DecodedMessage& msg);
    void print_message(const EncodedMessage& msg);
    void print_P_variant();
};
