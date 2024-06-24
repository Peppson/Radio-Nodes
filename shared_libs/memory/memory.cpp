
#include "memory.h"


void Memory::end() {
    EEPROM.end(); // void
}


void Memory::write_raid_variable(uint16_t value) {
    write<uint16_t>(RaidAddress::address_0, value);
    write<uint16_t>(RaidAddress::address_1, value);
}


uint16_t Memory::get_raid_variable() {
    constexpr uint16_t failed = 65535;
    uint16_t raid[2];

    // Read memory
    raid[0] = get<uint16_t>(RaidAddress::address_0);
    raid[1] = get<uint16_t>(RaidAddress::address_1);

    // Bank 0 failed, Bank 1 is good
    if (raid[0] == failed && raid[1] != failed) {
        write<uint16_t>(RaidAddress::address_0, raid[1]);

    // Bank 1 failed, Bank 0 is good
    } else if (raid[1] == failed && raid[0] != failed) { 
        write<uint16_t>(RaidAddress::address_1, raid[0]);
    
    // Both raids failed
    } else if (raid[0] == failed && raid[1] == failed) {
        write_raid_variable(0); // Initialize both back to 0
        return 0;
    }

    return raid[0];
}


void Memory::init_raid_variable(const uint16_t value) {
    // Init values once
    if (EEPROM.read(_init_bool_address) == 255) {
        EEPROM.write(_init_bool_address, 42);
        delay(5);

        // Write as RAID 1
        write_raid_variable(value);
    }
}
