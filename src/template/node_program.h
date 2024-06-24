
#pragma once
#include "../global_config.h"
#include "rf24_radio.h"
#include "hardware.h"
#include "water_system.h"

namespace selected_node {


void send_data_get_time(RF24Radio& radio, const bool request_time) {
    WDT_FEED();

    // Get sensor data
    radio.power_down();
    uint16_t water_level = WaterSystem::get_water_level();
    uint16_t battery_charge = Hardware::get_remaining_battery_charge(); 
    radio.power_up();

    // Request current time
    if (request_time) {
        bool success = (radio.send_message(0, water_level, battery_charge, 0xFFFF) 
            && radio.wait_for_message(3000)
            && Hardware::set_time_until_deepsleep(radio.message.data_2));

        if (!success) {
            Hardware::deepsleep(15*60);
            /*Reboot*/
        }
    // Data update
    } else {
        radio.send_message(0, water_level, battery_charge, 0);
    }
    
    // Set time until next update
    Hardware::send_data_timestamp = millis() + NODE_UPDATE_INTERVAL; 
}


void handle_message(RF24Radio& radio) {
    WDT_FEED();
    bool reboot = radio.message.data_2 == 0xFFFF;       // 65535
    bool resend_msg = radio.message.data_2 == 0xFFFE;   // 65534
    bool run_waterpump = radio.message.state;
    uint16_t pump_duration = radio.message.data_0;

    // Resend previous message
    if (resend_msg) {
        selected_node::send_data_get_time(radio, false);

    // Reboot   
    } else if (reboot) {
        Hardware::restart_device();

    // Run pump
    } else if (run_waterpump) {
        radio.power_down();
        WaterSystem::run_water_pump(pump_duration);

        // Flush buffers in case we recieved another message while pumping (most likely duplicate)
        if (radio.available()) { 
            radio.flush_tx();
            radio.flush_rx(); 
        }
    }
}
} // namespace selected_node
