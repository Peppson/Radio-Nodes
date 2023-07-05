
/*
#########################  NRF Node 1 Main  #########################
#                                                                   #
#   - Self watering flowerpot, controlled via MQTT/Radio gateway    #                                          
#   - NRF24L01+ Radio Address "1"                                   #     
#   - RF24_NODE_TYPE "1"                                            #
#   - Target Mcu ATtiny84A @ 1Mhz                                   #
#                                                                   #
#####################################################################
*/


// Main_Node_1.ino
#include "Config.h"
#include "Program.h"
class NRF24L01_radio RF24_radio;
class Hardware_class Hardware;


// Setup
void setup() { 
    Hardware.Setup();
    #if __ADC_CAL_ON__
        ADC_CAL_FUNC();
    #else
        Hardware.Is_watertank_empty();
        RF24_radio.Send_data_get_time();
        ADC_at_this_millis = millis() + UPDATE_INTERVAL;
    #endif     
}


// Main loop
void loop() {
    // Save power, toggle transciever on/off
    for (uint8_t i = 0; i <= MAIN_LOOP_ITERATIONS; i++) {
        uint16_t RF24_package[6];

        // Powerup radio, reset watchdog
        WDT_RESET();
        RF24_radio.powerUp();
        delay(2);
        RF24_radio.startListening();

        // Listen for incomming message, start waterpump?
        if (RF24_radio.Wait_for_message(200, RF24_package) 
        && ((RF24_package[0] == RF24_THIS_DEV_ADDRESS) 
        &&  (RF24_package[1] == RF24_MASTER_NODE_ADDRESS) 
        &&  (RF24_package[4] == true))) {
            Hardware.Start_water_pump(RF24_package[2]);
            RF24_radio.flush_rx(); 
            RF24_radio.flush_tx();
        }
        // Powerdown radio 
        RF24_radio.stopListening();
        if (i < MAIN_LOOP_ITERATIONS) {
            RF24_radio.powerDown();
            delay(RF24_SLEEP_TIME);
        }     
    } 
    #if __ATTINY84_ON__ && !__SERIAL_ON__
        unsigned long Current_millis = millis();
        
        // Time to deepsleep?
        if (Current_millis > Sleep_at_this_millis) {
            DEEPSLEEP(); 

        // Send node status every Update_interval
        } else if (Current_millis > ADC_at_this_millis) {
            // Memory related #if... 
            #if !__ADC_CAL_ON__ 
                RF24_radio.Send_data_get_time();
                ADC_at_this_millis = millis() + UPDATE_INTERVAL;
            #endif 
        }
    #endif    
}


