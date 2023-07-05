
/*
#####################  Master Node / MQTT Gateway  #####################
#                                                                      #
#   - MQTT Bridge for NRF24L01 nodes and generic 433Mhz radios          #                                          
#   - Library, TMRh20/RF24: https://github.com/tmrh20/RF24/            #     
#   - Class config: https://nrf24.github.io/RF24/classRF24.html        #     
#   - NRF24L01 Address "0"                                             #
#                                                                      #
########################################################################
*/


// Main.ino
#include "Config.h"
#include "Functions.h"
#include "Classes.h"


// Setup
void setup() {
    utils::Setup_hardware(); 
    MQTT.Get_current_time(); //TODO
    //println("start");
    //Debug();
    //setTime(1687509018); //TODO
}


void Debug() {
    /*
    unsigned long old_millis = millis();
    println(millis() - old_millis);
    delay(5000);
    */
    
}


// Main loop
void loop() {
    for (uint32_t i = 0; i < Main_loop_iterations; i ++) {
        
        // Reset RF24 Pipe and Check for new MQTT message
        uint8_t Pipe;
        MQTT.loop();

        // New RF24 radio message available? 
        if (RF24_radio.available(&Pipe) && Pipe == This_dev_address) {
            if (!RF24_radio.Get_available_message()) {
                delay(3000);
                RF24_radio.flush_rx();
                // Placeholder func "send again"
            }
            // Which Node was sending?
            else if (RF24_package[0] == This_dev_address) {
                RF24_radio.stopListening();
                utils::Print_package();
                RF24_radio.Respond_to_sending_node(RF24_package[1]);
                RF24_radio.flush_rx(); 
                RF24_radio.flush_tx();
                RF24_radio.startListening();
            }        
        }
    } 
    // MQTT connected?
    if (!MQTT.connected()) {
        for (uint8_t i = 0; i < 3; i ++) {
            if (MQTT.Connect()){
                break;
            }
            else if (i==2) {
                ESP.restart();
            }
            delay(5000);
        }   
    }
    // Grab time manually if something went wrong
    if (millis() - Prev_millis >= Check_time_interval) {
        MQTT.Get_current_time(); 
    }
    // Restart each monday at ca 04:00
    uint16_t Current_minute = hour() * 60 + minute();
    if ( (weekday() == 2) && (Current_minute >= 238 && Current_minute <= 239) ) {
        ESP.restart();
    }   
}




