

// Config.h
#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>
#include <RF24.h>  
#include "nRF24L01.h"  
#include <capacitor_lite.h> 


// User Controlled
#define ATTINY84_ENABLED 1                      // ATtiny84 or other MCUs
#define WATCHDOG_ENABLE 1                       // Enable watchdog     
#define SERIAL_ENABLED 0                        // Serial toggle
#define ADC_CAL_ENABLED 0                       // Enter battery ADC calibration (Req SERIAL_ENABLED)
#define PUMP_CAL_ENABLED 0                      // Enter pump ADC calibration (Req SERIAL_ENABLED)

#define RF24_THIS_DEV_ADDRESS 1                 // This device address
#define RF24_THIS_DEV_ADDRESS_STRING "1Adrs"    // Address string "1Adrs", "2Adrs", "10Adr", "11Adr"...
#define RF24_SLEEP_TIME 800                     // 200ms on, (ms) off                                   
#define RF24_CHANNEL 120                        // Channel 1-126 (RF_Scanner.ino)
#define RF24_OUTPUT (RF24_PA_MAX)               // Output level: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define RF24_DATARATE (RF24_250KBPS)            // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
#define RF24_PIPE 1                             // Pipe address (CONST)
#define RF24_MASTER_NODE_ADDRESS 0              // Address of master (CONST)

#define PUMP_RUNTIME_MAX 6                      // Maximum time pump is allowed to run (s)  
#define WATER_SENSOR_MIN_VALUE 70               // Calibrated with EMPTY tank, diy sensor mostly linear
#define WATER_SENSOR_MAX_VALUE 265              // Calibrated with FULL tank, diy sensor mostly linear

#define DEEPSLEEP_AT_THIS_TIME 2100             // Go to deepsleep at what time? (hhmm)
#define DEEPSLEEP_HOW_MANY_HOURS 12             // Deepsleep how long in (h)
#define UPDATE_INTERVAL 3*60*60*1000UL          // How often should the node send status update to master? (h*m*s*ms)
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
    #define ADC_MEASURE_PIN A7
    #define ADC_ENABLE_PIN PB1      // Ver 4.0 = PB1
    #define PUMP_PIN A0             // Ver 4.0 = A0
    #define WATER_SENSOR_PIN_1 PB2 
    #define WATER_SENSOR_PIN_2 PA1
    #define SERIAL_OUT_PIN PB0
    #define WDT_RESET() wdt_reset() 
    #define WDT_DISABLE() wdt_disable()
    #define WDT_ENABLE(x) wdt_enable(x)
    #define DEEPSLEEP(x) hardware.deepsleep(x);
#else
    #define CE_PIN 14       // Pico 14, pico zero 0, esp32 14
    #define CSN_PIN 5       // Pico 17, pico zero 1, esp32 5
    #define ADC_MEASURE_PIN 25
    #define ADC_ENABLE_PIN 26
    #define PUMP_PIN 2
    #define WATER_SENSOR_PIN_1 //PIN
    #define WATER_SENSOR_PIN_2 //PIN
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

#else // Cast print statements into nothing
    #define SERIAL_BEGIN(x)
    #define print(x)
    #define println(x)
    #define debug_print(x)
    #define debug_println(x)
#endif


