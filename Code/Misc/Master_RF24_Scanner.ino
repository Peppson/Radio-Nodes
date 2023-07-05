
#include <nRF24L01.h>  
#include "RF24.h"
#include "printf.h"
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h> 
#define SPI_2_MISO   12
#define SPI_2_MOSI   13
#define SPI_2_SCLK   14
#define SPI_2_CE     16 
#define SPI_2_CSN    5 
SPIClass spiBus2(HSPI); 


// MASTER
RF24 radio(16, 5);
const uint8_t num_channels = 126;
#define CHANNEL 120
const int num_reps = 50;
#define ON 1
bool do_once = 1;


// Globals
bool constCarrierMode = 0;
int values[num_channels];


// Setup
void setup(void) {
    Serial.begin(9600);
    printf_begin();
    spiBus2.begin(SPI_2_SCLK, SPI_2_MISO, SPI_2_MOSI, SPI_2_CE);
    while (!Serial) {} 
        for(uint8_t i = 0; i < 25; i++) {
            Serial.println();
        }
    Serial.println(F("\n\rRF24/examples/scanner/"));

    if (!(radio.begin(&spiBus2, SPI_2_CE, SPI_2_CSN) && radio.isChipConnected())) {
        Serial.println("radio hardware not responding!");
        while (1) {} 
    }
    // Radio config
    radio.setPALevel(RF24_PA_MAX, 1);
    radio.setAutoAck(false);
    radio.startListening();
    radio.stopListening();
    radio.printDetails();
}


// Main loop
void loop(void) {
        if (ON == 1 && do_once == 1) { 
            radio.stopListening();
            delay(2);
            do_once = 0;
            constCarrierMode = 1;
            Serial.println("Starting Carrier Out");
            radio.startConstCarrier(RF24_PA_MAX, CHANNEL);
        } else  if (ON == 0 && do_once == 1){
            do_once = 0;
            constCarrierMode = 0;
            radio.stopConstCarrier();
            Serial.println("Stopping Carrier Out");
        }
    
    /****************************************/

    if (constCarrierMode == 0) {
        // Clear measurement values
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
