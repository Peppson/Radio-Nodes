


// Config.h
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include <Arduino.h>
#include <stdint.h>
#include <SPI.h>  
#include <nRF24L01.h>  
#include <RF24.h>
#include <EEPROM.h> 


// User Controlled
#define __ATTINY84_ON__ 1                   // ATtiny84 or other MCUs   
#define __SERIAL_ON__ 0                     // Serial toggle
#define __ADC_CAL_ON__ 0                    // Enter ADC_CAL_FUNC (Req __SERIAL_ON__)

#define RF24_MASTER_NODE_ADDRESS 0          // Address of master
#define RF24_THIS_DEV_ADDRESS 1             // This device address
#define RF24_THIS_DEV_ADDRESS_STR "1Adrs"   // Address string 
#define RF24_NODE_TYPE 1                    // 1 = Self watering plants, 2 = ...
#define RF24_SLEEP_TIME 800                 // 200ms on, (ms) off                                   
#define RF24_CHANNEL 120                    // Channel 1-126 (RF_Scanner.ino)
#define RF24_OUTPUT (RF24_PA_MAX)           // Output level: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX
#define RF24_DATARATE (RF24_250KBPS)        // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS

#define PUMP_TIMES_UNTIL_EMPTY 6            // How many times the pump can run until empty tank (measure, dependant on Pump_time)
#define PUMP_TIMES_MAX 12                   // Maximum time pump is allowed to run (s)
#define PUMP_LEVEL_EEPROM_ADDRESS 0         // Address of eeprom memory which stores Pump_counter 
#define PUMP_TANK_EEPROM_ADDRESS 1          // Address of eeprom memory which stores Is_watertank_empty            
#define DEEPSLEEP_AT_THIS_TIME 2100         // Go to deepsleep at what time? (hhmm)
#define DEEPSLEEP_HOW_LONG 12               // Deepsleep how long in (h)
#define UPDATE_INTERVAL 3*60*60*1000UL      // How often should the node report its battery and waterlevel status? (h*m*s*ms)
#define MAIN_LOOP_ITERATIONS 20             // How many loop iterations 


// Board Select
#if __ATTINY84_ON__ 
    #include <ATTinyCore.h>
    #include <avr/wdt.h>
    #include <avr/sleep.h>
    #define CE_PIN A2         
    #define CSN_PIN A3
    #define PUMP_PIN A0             // Ver 3.0 = A0, ver 2.0 = PB2
    #define PUMP_WATERTANK_PIN PB2  // Check version
    #define ADC_MEASURE_PIN A7
    #define ADC_ENABLE_PIN A1
    #define DEEPSLEEP() Hardware.Deepsleep();
    #define WDT_RESET() wdt_reset() 
    #define WDT_DISABLE() wdt_disable()
    #define WDT_ENABLE(x) wdt_enable(x)
#else
    #define CE_PIN 14           // Pico 14, pico zero 0, esp32 14
    #define CSN_PIN 5           // Pico 17, pico zero 1, esp32 5
    #define PUMP_PIN 2
    #define PUMP_WATERTANK_PIN 16        
    #define ADC_MEASURE_PIN 25
    #define ADC_ENABLE_PIN 26
    #define DEEPSLEEP()
    #define WDT_RESET()
    #define WDT_DISABLE()
    #define WDT_ENABLE(x)
#endif


// Serial toggle
#if __SERIAL_ON__ && __ATTINY84_ON__
    #include <SendOnlySoftwareSerial.h>
    extern SendOnlySoftwareSerial Tiny_serial;
    #define print(x) Tiny_serial.print(x)
    #define println(x) Tiny_serial.println(x)
    #define Debug_print(x)
    #define Debug_println(x)
    #define SERIAL_BEGIN(x) Tiny_serial.begin(x)

#elif __SERIAL_ON__ && !__ATTINY84_ON__
    #define print(x) Serial.print(x)
    #define println(x) Serial.println(x)
    #define Debug_print(x) print(x)
    #define Debug_println(x) println(x)
    #define SERIAL_BEGIN(x) Serial.begin(x)

#else // Cast print statements into nothing
    #define print(x)
    #define println(x)
    #define Debug_print(x)
    #define Debug_println(x)
    #define SERIAL_BEGIN(x)
#endif


// Globals
extern unsigned long Sleep_at_this_millis;  // Absolute timestamp
extern unsigned long ADC_at_this_millis;    // Absolute timestamp
extern volatile int  Deepsleep_count;       // Counter while in deepsleep

#endif // __CONFIG_H__


