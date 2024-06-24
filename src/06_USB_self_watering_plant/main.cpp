
#include "../global_config.h"
#include "node_program.h"
#include "rf24_radio.h"
#include "hardware.h"
#include "water_system.h"
#include "utility.h"

RF24Radio radio;
Hardware hardware(PIN_PA4, PIN_PA5);
WaterSystem water_system(PIN_PA6, PIN_PA7, selected_node::WATER_SENSOR_MIN_VALUE, selected_node::WATER_SENSOR_MAX_VALUE);


#if CALIBRATION
void setup() { Hardware::begin(&radio); util::calibration(); }
#else


void setup() {
    hardware.begin(&radio);
    radio.begin();
    selected_node::send_data_get_time(radio, true);
}


void loop() {
    WDT_FEED();

    // Save power by toggling radio transceiver on/off
    radio.power_up();

    // Listen for radio message
    if (radio.wait_for_message(RF24_ON_TIME)) {
        selected_node::handle_message(radio);
    }

    // Powerdown radio 
    radio.power_down();
    hardware.lightsleep(RF24_OFF_TIME);
    uint32_t now_timestamp = millis();

    // Time to Send data to master?
    if (now_timestamp > hardware.send_data_timestamp) {
        selected_node::send_data_get_time(radio, false);
    } 

    // Time to sleep?
    if (now_timestamp > hardware.deepsleep_timestamp) {
        hardware.deepsleep(selected_node::SLEEP_DURATION_SECONDS);
    } 
}
#endif   
