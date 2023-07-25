

// Config.h
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <RF24.h>  
#include "nRF24L01.h"
#include <EEPROM.h>  


// User Controlled
#define ATTINY84_ENABLED 1                      // ATtiny84 or other MCUs
#define WATCHDOG_ENABLE 1                       // Enable watchdog    
#define SERIAL_ENABLED 0                        // Serial toggle
#define ADC_CAL_ENABLED 0                       // Enter battery ADC calibration (Req SERIAL_ENABLED)
#define COFFEE_COUNT 150                        // Starting coffee_count value, needs to be set at uploading (Overflows @ 4095)

#define RF24_THIS_DEV_ADDRESS 11                // This device address
#define RF24_THIS_DEV_ADDRESS_STRING "11Adr"    // Address string "1Adrs", "2Adrs", "10Adr", "11Adr"...
#define RF24_SLEEP_TIME 800                     // 200ms on, (ms) off                                   
#define RF24_CHANNEL 120                        // Channel 1-126 (RF_Scanner.ino)
#define RF24_OUTPUT (RF24_PA_MAX)               // Output level: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define RF24_DATARATE (RF24_250KBPS)            // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
#define RF24_PIPE 1                             // Pipe address (CONST)
#define RF24_MASTER_NODE_ADDRESS 0              // Address of master (CONST)

#define COFFEE_ADC_THRESHOLD 100                // Current sensor threshold 
#define SERVO_ON_POS 550                        // On position of servo
#define SERVO_ON_MIDDLE_POS 800                 // Middle position after ON of servo
#define SERVO_OFF_POS 1300                      // Off position of servo
#define SERVO_OFF_MIDDLE_POS 950                // Middle position after OFF of servo

#define DEEPSLEEP_AT_THIS_TIME 2200             // Go to deepsleep at what time? (hhmm)
#define DEEPSLEEP_HOW_MANY_HOURS 8              // Deepsleep how long in hours
#define LOOP_ITERATIONS 20                      // Main loop iterations (max uint8_t)


// Global counter while in deepsleep
extern volatile int deepsleep_counter;


// Board Select
#if ATTINY84_ENABLED 
    #include <ATTinyCore.h>
    #include <avr/wdt.h>
    #include <avr/sleep.h>
    #define CE_PIN A2         
    #define CSN_PIN A3
    #define ADC_MEASURE_PIN A0          
    #define SERVO_PIN PB1                
    #define SERIAL_OUT_PIN PB0
    #define WDT_RESET() wdt_reset() 
    #define WDT_DISABLE() wdt_disable()
    #define WDT_ENABLE(x) wdt_enable(x)
    #define DEEPSLEEP(x) hardware.deepsleep(x);
#else
    #define CE_PIN 14       // Pico 14, pico zero 0, esp32 14
    #define CSN_PIN 5       // Pico 17, pico zero 1, esp32 5
    #define ADC_MEASURE_PIN 25
    #define WDT_RESET()
    #define WDT_DISABLE()
    #define WDT_ENABLE(x)
    #define DEEPSLEEP(x)
#endif


// Serial toggle
#if SERIAL_ENABLED && ATTINY84_ENABLED
    #include <SendOnlySoftwareSerial.h>
    extern SendOnlySoftwareSerial tiny_serial;
    #define SERIAL_BEGIN(x) tiny_serial.begin(x)
    #define print(x) tiny_serial.print(x)
    #define println(x) tiny_serial.println(x)
    #define debug_print(x)
    #define debug_println(x)

#elif SERIAL_ENABLED && !ATTINY84_ENABLED
    #define SERIAL_BEGIN(x) Serial.begin(x)
    #define print(x) Serial.print(x)
    #define println(x) Serial.println(x)
    #define debug_print(x) print(x)
    #define debug_println(x) println(x)
    #define debug_println(x) println(x)

#else // Cast print statements into nothing
    #define SERIAL_BEGIN(x)
    #define print(x)
    #define println(x)
    #define debug_print(x)
    #define debug_println(x)   
#endif


