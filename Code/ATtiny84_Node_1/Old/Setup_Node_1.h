
/* 
##########################  Node 1 Setup  ###########################
# - ATtiny84                                                        #
#                        VCC  1|o   |14 GND                         #
#                        10   2|    |13 A0/0 (AREF)                 #
#                        9    3|    |12 A1/1 --- ADC_Enable_Pin     #
#         Pullup 10K --- RST  4|    |11 A2/2 --- Radio   CE         #
#           PUMP_PIN --- PB2  5|    |10 A3/3 --- Radio  CSN         #
#   ADC_Measure_PIN --- A7/7  6|    |9  A4/4 --- Radio  SCK         #
#        Radio MISO --- A6/6  7|    |8  A5/5 --- Radio MOSI         #
#                                                                   #
# - Pico zero pinout                                                #
# - gp0 = CE, gp1 = CSN, gp2 = SCK, gp3 = MOSI, gp4 = MISO          #
#                                                                   #
#####################################################################
*/

// Setup_Node_1
#pragma once
#include <SPI.h>  
#include <nRF24L01.h>  
#include <RF24.h>
#include <stdint.h>


// User Controlled
#define ATtiny84_ON 1                       // ATtiny84 or pico    
#define SERIAL_ON 0                         // Serial toggle
#define ADC_CAL_ON 0                        // Enter ADC_CAL_FUNC (req SERIAL_ON)

#define This_dev_address 1                  // Device address 
#define Master_node_address 0               // Address of master
#define Sleep_radio_for_ms 800              // 200ms on, (ms) off                                   
#define Radio_channel 124                   // Radio channel
#define Radio_output RF24_PA_MIN            // Output level: RF24_PA_MIN, RF24_PA_LOW, RF24_PA_HIGH, RF24_PA_MAX

#define Pump_runtime_max 15                 // Maximum time pump is allowed to run (s)              
#define Sleep_time 2100                     // Go to deepsleep at what time? (hhmm)
#define Sleep_how_long 12                   // Deepsleep how long in (h)
#define Update_interval 3*60*60*1000UL      // How often should the node report its battery status? (h*m*s*ms)
#define Main_loop_iterations 20             // How many loop iterations 


// Board Select
#if ATtiny84_ON 
    #include <ATTinyCore.h>
    #include <avr/wdt.h>
    #include <avr/sleep.h> 
    #define CE_PIN A2         
    #define CSN_PIN A3
    #define PUMP_PIN PB2   
    #define ADC_Measure_PIN A7
    #define ADC_Enable_Pin A1
    #define Deepsleep_device() Deepsleep();
    #define WDT_RESET() wdt_reset() 
#else
    #define CE_PIN 0        // Pico 14, arduino 9, pico zero 0
    #define CSN_PIN 1       // Pico 17, arduino 10, pico zero 1
    #define PUMP_PIN 15        
    #define ADC_Measure_PIN 29
    #define Deepsleep_device() Fake_deepsleep();
    #define WDT_RESET()
#endif


// Serial toggle
#if SERIAL_ON
    #if ATtiny84_ON
        #include <SendOnlySoftwareSerial.h>
        SendOnlySoftwareSerial mySerial(0);
        #define println(x) mySerial.println(x)
        #define print(x) mySerial.print(x)
        #define SERIAL_BEGIN(x) mySerial.begin(x);
    #else
        #define println(x) Serial.println(x)
        #define print(x) Serial.print(x)
        #define SERIAL_BEGIN(x) Serial.begin(x); 
    #endif
#else
    #define println(x)
    #define print(x)
    #define SERIAL_BEGIN(x) 
#endif


// Globals
unsigned long
ADC_at_this_millis,                         // Init
Current_millis,                             // Init
Sleep_at_this_millis;                       // Init

volatile int 
Deepsleep_count = 0;                        // Counter in deepsleep      

// Message package                          
uint16_t Msg_to_who;                        // Attiny84 have 2 bytes for Tx and Rx buffers...
uint16_t Msg_from_who;                      // Splitting up package into small chunks of data 
uint16_t Msg_int;                           // uint16_t (2B) is max
uint16_t Msg_float; 
uint16_t Msg_state; 
uint16_t Msg_time;
uint16_t Message_package[6] = {Msg_to_who, Msg_from_who, Msg_int, Msg_float, Msg_state, Msg_time};                           

// Node address array  
const uint8_t address[5][6] = {"1Adrs", "2Adrs", "3Adrs", "4Adrs", "5Adrs"}; 


// Radio object
RF24 radio(CE_PIN, CSN_PIN);  

