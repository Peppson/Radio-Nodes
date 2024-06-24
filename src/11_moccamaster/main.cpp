
/*
#########################  Radio Node 11  #########################
#                                                                 #
#  - Mocca Master Controller, controlled via MQTT Radio gateway   #                                          
#  - NRF24L01+ Radio Address "11"                                 #     
#  - Target Mcu = ATtiny84A @ 4Mhz                                #
#                                                                 #
###################################################################
*/


// Main.cpp
#include "config.h"
#include "program.h"
class RF24Radio radio_24;
class HardwareClass hardware;


// Setup
void setup() { 
    hardware.begin();
    #if ADC_CAL_ENABLED
        extern void adc_calibration(); 
        adc_calibration();
    #else
        hardware.coffee_count = hardware.read_value_from_EEPROM();
        hardware.coffee_state = hardware.get_mocca_master_state();
        hardware.move_servo(0, true);
        radio_24.send_data_get_time();
    #endif  
}


// Main loop
void loop() {
    // Save power, toggle radio transciever on/off
    for (uint8_t i = 0; i < LOOP_ITERATIONS; i++) {
        uint16_t rf24_package[6] = {};
        uint32_t current_timestamp = millis();

        // Powerup radio, reset watchdog
        WDT_RESET();
        radio_24.powerUp();
        delay(5);
        radio_24.startListening();

        // Listen for incomming message
        if (radio_24.wait_for_message(200, rf24_package)) {
            hardware.mocca_master_control(rf24_package);
        }
        // Periodically check Mocca master state
        if (current_timestamp > hardware.prevoius_timestamp + 1500) {  
            hardware.prevoius_timestamp = millis();

            // Have coffee_state changed seens last check?
            bool new_coffee_state = hardware.get_mocca_master_state();
            if (new_coffee_state != hardware.coffee_state) {

                // Increment coffee_count only if turned on (from off to on)
                if (new_coffee_state && !hardware.coffee_state) { 
                    hardware.coffee_count += 1;
                    hardware.update_value_to_EEPROM();
                }
                // Update coffee_state and send status update to master
                hardware.coffee_state = new_coffee_state;
                radio_24.try_send_message();
            }
        }
        // Powerdown radio and "sleep" (blocking)
        radio_24.stopListening();
        radio_24.powerDown();
        delay(RF24_SLEEP_TIME);   
    }
    // Time for deepsleep?
    if (millis() > hardware.sleep_at_this_timestamp) { DEEPSLEEP(); } 
}


