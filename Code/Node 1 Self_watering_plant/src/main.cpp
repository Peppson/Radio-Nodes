
/*
##########################  Radio Node 1  ##########################
#                                                                  #
#   - Self watering flowerpot, controlled via MQTT Radio gateway   #                                          
#   - NRF24L01+ Radio Address "1"                                  #     
#   - Target Mcu = ATtiny84A @ 4Mhz                                #
#                                                                  #
####################################################################
*/


// main.cpp
#include "config.h"
#include "program.h"
class RF24Radio radio_24;
class HardwareClass hardware;
class CapacitorLite water_sensor(WATER_SENSOR_PIN_1, WATER_SENSOR_PIN_2);
 

// Setup
void setup() { 
    hardware.setup();
    #if ADC_CAL_ENABLED
        extern void adc_calibration(); 
        adc_calibration();
    #elif PUMP_CAL_ENABLED
        extern void pump_calibration();
        pump_calibration();
    #else
        radio_24.send_data_get_time();
        hardware.send_data_timestamp = millis() + UPDATE_INTERVAL;
    #endif  
}


// Main loop
void loop() {
    // Save power, toggle radio transciever on/off
    for (uint8_t i = 0; i < LOOP_ITERATIONS; i++) {
        uint16_t rf24_package[6] = {};

        // Powerup radio, reset watchdog
        WDT_RESET();
        radio_24.powerUp();
        delay(5);
        radio_24.startListening();

        // Listen for incomming message
        if (radio_24.wait_for_message(200, rf24_package)) {
            hardware.water_pump_control(rf24_package);
        }
        // Powerdown radio 
        radio_24.stopListening();
        if (i < LOOP_ITERATIONS) {
            radio_24.powerDown();
            delay(RF24_SLEEP_TIME);
        }     
    } 
    // Memory related #if... 
    #if !ADC_CAL_ENABLED 
        uint32_t current_timestamp = millis();
        
        // Time for deepsleep?
        if (current_timestamp > hardware.sleep_at_this_timestamp) {
            DEEPSLEEP(); 

        // Send node data to master every UPDATE_INTERVAL
        } else if (current_timestamp > hardware.send_data_timestamp) {
            hardware.send_data_timestamp = millis() + UPDATE_INTERVAL; 
            radio_24.send_data_get_time();   
        }
    #endif  
}


