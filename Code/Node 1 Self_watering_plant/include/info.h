
/* 
#######################  Attiny84A Pinout  #########################
#                                                                  #
#                        VCC  1|o   |14 GND                        #
#  (Tiny_serial out) --- PB0  2|    |13 A0/0 --- PUMP_PIN          #
#     ADC_Enable_Pin --- PB1  3|    |12 A1/1 --- Sensor_pin (-)    #
#      Pullup R 100K --- RST  4|    |11 A2/2 --- RF24 CE           #
#     Sensor_pin (+) --- PB2  5|    |10 A3/3 --- RF24 CSN          #
#   ADC_MEASURE_PIN --- A7/7  6|    |9  A4/4 --- RF24 SCK          #
#         RF24 MISO --- A6/6  7|    |8  A5/5 --- RF24 MOSI         #
#                                                                  #
####################################################################


######################  RF24 message package  ######################
#                                                                  #
# - ATtiny84A have 2 bytes for Tx and Rx buffers...                #
# - splitting up package into small chunks of data                 #
#                                                                  #
# - uint16_t RF24_package[6] = {}                                  #
# - RF24_package[0] = to_who                                       #
# - RF24_package[1] = from_who                                     #
# - RF24_package[2] = "int"                                        #
# - RF24_package[3] = "float"                                      #
# - RF24_package[4] = "bool"                                       #
# - RF24_package[5] = "time"                                       #
#                                                                  #
####################################################################
 










// Check EEPROM memory for errors 
    //check_EEPROM_on_boot(); //TODO


// Read from EEPROM memory
uint16_t HardwareClass::read_value_from_EEPROM() {

    // Read bank_0 and bank_1, EEPROM = 1 byte each
    uint8_t bank_0 = EEPROM.read(0);
    uint8_t bank_1 = EEPROM.read(1);

    return (bank_0 * 256) + bank_1;   
}


// Write to EEPROM memory
void HardwareClass::update_value_to_EEPROM() {

    uint8_t bank_0 = hardware.coffee_count / 256;
    uint8_t bank_1 = hardware.coffee_count % 256;
    
    // Write bank_0 and bank_1 as RAID 1
    for (uint8_t i = 0; i < 4; i += 2) {
        EEPROM.update(i, bank_0);
        EEPROM.update(i + 1, bank_1);
    }
}


// EEPROM memory checks on boot. Uninitialized memory = 255
void HardwareClass::check_EEPROM_on_boot() {
    
    // Set starting value for coffee_count only once, ever.
    if (EEPROM.read(5) == 255) {
        EEPROM.write(5, 42); // Random value

        uint8_t bank_0 = COFFEE_COUNT / 256;
        uint8_t bank_1 = COFFEE_COUNT % 256;

        // Write bank_0 and bank_1 as RAID 1
        for (uint8_t i = 0; i < 4; i += 2) {
            EEPROM.update(i, bank_0);
            EEPROM.update(i + 1, bank_1);
        }
    }
    // Read banks from EEPROM                                    
    uint8_t bank[4] = {};                                   
    for (uint8_t i = 0; i < 4; i++) {                                                                        
        bank[i] = EEPROM.read(i);                                           
    }   
    // Any memory errors? check each bank against its copy (0-2, 1-3)
    for (uint8_t i = 0; i < 2; i += 1) {
        if (bank[i] != bank[i + 2]) {
            uint8_t valid_bank_index = (bank[i] < bank[i + 2]) ? (i) : (i + 2);

            // Copy contents of the valid bank to the failed bank
            uint8_t failed_bank_index = (valid_bank_index == i) ? (i + 2) : i;
            bank[failed_bank_index] = bank[valid_bank_index];
        }
    }
    // All memory banks failed? Set all banks to 0. Error Accounted for in Node red
    uint16_t sum = 0;
    for (uint8_t i = 0; i < 4; i++) { 
        sum += bank[i]; 
    }
    if (sum == 255 * 4) {
        for (uint8_t i = 0; i < 4; i++) { 
            bank[i] = 0;
        }    
    }
}


*/


