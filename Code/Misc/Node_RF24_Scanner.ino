
#include "RF24.h"
#include "printf.h"
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>  

#define CARRIER_ON 1
#define CHANNEL 120
const uint8_t num_channels = 126;
const int num_reps = 50;


RF24 radio(14, 5);
int values[num_channels];
bool do_once = true;


// Setup
void setup(void) {
    Serial.begin(9600);
    printf_begin();
    delay(1000);
    while (!Serial) {} 
        for(uint8_t i = 0; i < 25; i++) {
            Serial.println();
        }
    Serial.println(F("\n\rRF24/examples/scanner/"));

    if (!radio.begin()) {
        Serial.println("radio hardware not responding!");
        while (1) {} 
    } else {
        Serial.println("Radio OK!");
    }
    radio.setAutoAck(false);
    radio.startListening();
    radio.stopListening();
    radio.printDetails();
}
    

// Main loop
void loop(void) {
    if (CARRIER_ON && do_once) {
        do_once = false;
        radio.stopListening();
        delay(2);
        Serial.println("Starting Carrier Out");
        radio.startConstCarrier(RF24_PA_HIGH, CHANNEL);
    } 

    if (!CARRIER_ON) {
        memset(values, 0, sizeof(values));

        // Scan all channels num_reps times
        int rep_counter = num_reps;
        while (rep_counter--) {
            int i = num_channels;
            while (i--) {
                // Select this channel
                radio.setChannel(i);

                // Listen for a little
                radio.startListening();
                delayMicroseconds(128);
                radio.stopListening();

                // Did we get a carrier?
                if (radio.testCarrier()) {
                ++values[i];
                }
            }
        }

        // Print out channel measurements, clamped to a single hex digit
        int i = 0;
        while (i < num_channels) {
        if (values[i]) {
            Serial.print(min(0xf, values[i]), HEX);
        } else {
            Serial.print(F("-"));
        }
        ++i;
        }
        Serial.println();
    }  
}


