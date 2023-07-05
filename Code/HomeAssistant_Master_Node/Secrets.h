
/*
########################  Secrets.h  #########################
*/

// Don't look :(
#pragma once 
#include <Arduino.h>


// MQTT
const char* Secret_MQTT_ssid = "";
const char* Secret_MQTT_password = "";
const char* Secret_MQTT_ip = "";


// RF433
const uint16_t Secret_RF_remote_codes[7][132] PROGMEM = {

    // Button 0 ON
        {111, 111, 111}, 
    // Button 0 OFF
        {111, 111, 111},
    // Button 1 ON
        {111, 111, 111},    
    // Button 1 OFF
        {111, 111, 111},
    // Button 2 ON
        {111, 111, 111},
    // Button 2 OFF
        {111, 111, 111},
    // Button ALL OFF
        {111, 111, 111},
};










