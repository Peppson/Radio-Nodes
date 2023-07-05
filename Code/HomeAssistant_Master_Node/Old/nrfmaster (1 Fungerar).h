
/*
############################ NRF 0 Master ############################
#                                                                    #
#   - EspHome, NRF24L01 bridge to Home Assistant                     #                                          
#   - Library, TMRh20/RF24: https://github.com/tmrh20/RF24/          #     
#   - Class config: https://nrf24.github.io/RF24/classRF24.html      #     
#   - NRF24L01 Address "0"                                           #
#                                                                    #
######################################################################
*/


// Set to 1 when compiling for ESP_HOME device
#define ESP_HOME 1

#include <nRF24L01.h>  
#include <RF24.h>
#define CE_PIN 6
#define CSN_PIN 5

// Address array for "This_dev_address". Must be declared before "class nrfmaster"?????:O
const uint8_t address[][6] = 
{ "1Node", "2Node", "3Node", "4Node", "5Node" };     

// Specifics for ESPHome 
#if ESP_HOME 
    #include "esphome.h"
    #define print(x) ESP_LOGD("custom_component", x);
    class nrfmaster : public Component, public CustomAPIDevice {
    public:
#else
    #define print(x) Serial.println(x);
#endif



// Variables
bool 
Do_once_every_12h = true;               // Init

uint8_t                                    
Pump_time = 2,                          // Node 1 variable
Sender_address,                         // Init
This_dev_address = 0;                   // Device address 0 = master            User Controlled

unsigned long
NTP_time = 0,                           // Init
Prev_millis,                            // Init
Current_millis;                         // Init

const unsigned long
ADC_interval = 1000UL*60*60*2;          // How often send ADC data?             User Controlled


// Setup
//RF24 radio(CE_PIN, CSN_PIN);  // Init radio object
#if ESP_HOME
void setup() override {
    register_service(&nrfmaster::NRF_node_1, "version_3", {"Pump_time"});                       // Create Bridge to HA 
    //register_service(&nrfmaster::NRF_node_2, "NRF_node_2", {"","",""...});                    // From HA to dev 
    //call_homeassistant_service("service_name", {{"what field", "text input"}});               // From dev to HA
    // call_homeassistant_service("notify.mobile_app_iphone_jarka", {{"message", "yep!"}});     // Test
#else
    void setup() {
    Serial.begin(9600);
#endif

// void setup() {

    /*
    // Radio init
    radio.begin();
    delay(150);
    radio.setPALevel(RF24_PA_MIN);                                          // Transmitter strength   
    radio.setChannel(124);                                                  // Above Wifi 
    radio.setDataRate(RF24_1MBPS);                                          // RF24_2MBPS, RF24_1MBPS, RF24_250KBPS                                      
    
    
    radio.openReadingPipe(This_dev_address, address[This_dev_address]);     // What pipe to listen on ALL
    radio.startListening();
    
    // Set the addresses for all pipes to TX nodes
    for (uint8_t i = 0; i < 5; ++i)
      radio.openReadingPipe(i, address[i]);
    */

    // Misc
    Current_millis = millis();
    Prev_millis = Current_millis;
}


// Send message 
bool Send_message(uint8_t To_which_node = 0, bool On_off = false) {
    switch (To_which_node) {

        // To NRF node 1 
        case 1:
            struct Tx_package {                                
            uint8_t from;                   // From who                              
            uint8_t Pump_T;                 // How long should the pump run for                             
            uint8_t NTP_Hour;               // Unix NTP hour 0-23                            
            bool Pump_On;                   // On/off                              
            };
            // Insert variables
            Tx_package Tx_data;
            Tx_data.from = This_dev_address;         
            Tx_data.Pump_T = Pump_time;              
            Tx_data.NTP_Hour = (NTP_time / 3600) % 24;             
            Tx_data.Pump_On = On_off;             
            break;
        
        // To NRF node 2 
        case 2:
            break;  

        default:
            return false;
            break;       
    }
    // Send message
    //radio.openWritingPipe(address[To_which_node]);
    //return radio.write( &Tx_data, sizeof(Tx_package) );
    return true;    
}


// Try to send message 3 times
bool Try_send_message(uint8_t To_which_node = 0, bool On_off = false){
  for (int i = 0; i < 3; i++) {
      if (Send_message(To_which_node, On_off)) {
        return true;
      } 
      delay(75); // Eventually falls inside a nodes "listening window"
  }
  return false;
}


// Grab current time from Home Assistant every 12h
void Get_current_time() {
    if ( Do_once_every_12h && (millis() / 1000) >= 40 ) {
        #if ESP_HOME
            auto NTP_time = id(esptime).utcnow();
        #endif
        Do_once_every_12h = false;
        Prev_millis = (millis());
    }
}



// NRF_node_1 start, call from Home Assistant
void NRF_node_1(int Pump_time) {

    //radio.stoptListening();
    //if (Try_send_message(1, true)) {
        //print("Message successful");
    //}
    //radio.startListening();

    if (Pump_time == 1) {
        print("Number is 1");
    } else if (Pump_time == 2) {
        print("Number is 2");
    } else {
        print("annars");
        //call_homeassistant_service("notify.mobile_app_iphone_jarka", {{"message", "yep!"}});   // test    
    }
}


/*
// NRF_node_2 start, call from Home Assistant
void NRF_node_2(<variable>) {
}
*/



// Main loop
#if ESP_HOME
    void loop() override {
#else
    void loop() {
#endif

// void loop() {

    print(".");
    delay(2000);
    
    // Grab current time from Home Assistant every 12h
    if (Do_once_every_12h && millis() >= 50*1000) {
        Get_current_time();
    }
    // Reset Do_once_every_12h, has to be like this for esphome to work...
    if (millis() >= (Prev_millis + 1000*60*60*12)) {
        Do_once_every_12h = true;
    }
}


// For EspHome Class
#if ESP_HOME
    };
#endif

    //ESP_LOGD("custom_component", "My: %lld", NTP_time);
