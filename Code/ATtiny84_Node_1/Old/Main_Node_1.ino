
/*
###########################  Node 1 Main  ###########################
#                                                                   #
#   - Flower_waterpump, controlled via Home Assistant               #                                          
#   - Library, TMRh20/RF24: https://github.com/tmrh20/RF24/         #     
#   - Class config: https://nrf24.github.io/RF24/classRF24.html     #     
#   - NRF24L01 Address "1"                                          #
#                                                                   #
#####################################################################
*/

// Main_Node_1
#include "Setup_Node_1.h"
#include "Functions_Node_1.h"


// Setup
void setup() { 
    Setup_everything();
    #if ADC_CAL_ON
        ADC_CAL_FUNC();
    #else
        Send_ADC_get_time();
        ADC_at_this_millis = millis() + Update_interval;
    #endif 
}


// Main loop
void loop() {
    // Save power, toggle transciever on/off
    for (uint16_t i = 0; i <= Main_loop_iterations; i++) { 

        // Powerup radio, reset watchdog
        WDT_RESET();
        radio.powerUp();
        delay(2);
        radio.startListening();

        // Listen for incomming messages
        if (Wait_for_message(200)) {
            println("Msg received!");
            // Start waterpump?
            if ((Message_package[0] == This_dev_address) && (Message_package[1] == Master_node_address) && (Message_package[4] == 1)) {
                Start_water_pump(Message_package[2]);
                radio.flush_rx(); 
                radio.flush_tx();   
            }
        }
        // Powerdown radio 
        radio.stopListening();
        if (i < Main_loop_iterations) {
            radio.powerDown();
            delay(Sleep_radio_for_ms);
        }     
    }
    // Time to sleep?
    Current_millis = millis();
    if (Current_millis > Sleep_at_this_millis) {
        Deepsleep_device();
    }
    // Send battery status every Update_interval
    else if (Current_millis > ADC_at_this_millis) {
        #if !ADC_CAL_ON
            Send_ADC_get_time();
            ADC_at_this_millis = millis() + Update_interval;
        #endif
    }    
}

