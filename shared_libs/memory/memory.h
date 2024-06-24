
#pragma once
#include "../global_config.h"
#include <EEPROM.h>


class Memory {
public:
    Memory() { EEPROM.begin(); };
    ~Memory() { end(); };
    
    static void end();
    static uint16_t get_raid_variable();
    static void write_raid_variable(const uint16_t value);
    static void init_raid_variable(const uint16_t value);

    // EEPROM = 128 banks of 1 Byte in size
    // Template used for storing bigger nums, across multiple banks
    template <typename T = uint8_t>
    T static get(const uint8_t address) {
        constexpr size_t size = sizeof(T);

        // Hold the bytes from EEPROM and interpret them as type T
        union { 
            T number;
            uint8_t bytes[size];
        } data;
        
        // Insert each EEPROM Byte into union 
        for (size_t i = 0; i < size; i++) {
            data.bytes[i] = EEPROM.read(address + i);
        }
        return data.number;
    }

    template <typename T = uint8_t>
    void static write(const uint8_t address, const T& value) {
        constexpr size_t size = sizeof(T);

        // Store the number across multiple 1-byte wide memory banks
        union { 
            T number;
            uint8_t bytes[size];
        } data;
        data.number = value;
        
        // Write each byte to EEPROM
        for (size_t i = 0; i < size; i++) {
            EEPROM.update(address + i, data.bytes[i]);
        }
    }

private:
    enum RaidAddress : uint8_t {
        address_0 = 123,   // 2B (uint16_t)
        address_1 = 125    // 2B (uint16_t)   
    };
    static constexpr uint8_t _init_bool_address = 127; 
};
