
/*
########################  Classes.h  #########################
*/


#pragma once
#include "Config.h"
#include "Functions.h"


// Egen fil???
void Transmit_RF_remote_codes(uint8_t button, uint8_t retries = 3);
namespace utils { uint16_t Calc_remaining_battery_charge(); }



// NRF24L01 Radio
class NRF24L01_radio : public RF24 {
    private:
        // Radio device addresses     
        const uint8_t address[5][6] = {"1Adrs", "2Adrs", "3Adrs", "4Adrs", "5Adrs"}; 
        
    public:
        // Constructor 
        NRF24L01_radio(uint8_t CE = SPI_2_CE, uint8_t CSN = SPI_2_CSN) : RF24(CE, CSN) {};

        // Begin
        bool Begin_object();

        // Send message 
        bool Send_message(uint16_t Local_RF24_package[6]); 

        // Try to send message (param hell)
        bool Try_send_message(uint16_t arg_to_who = 0, uint16_t arg_int = 0, uint16_t arg_float = 0, uint16_t arg_state = 0, uint16_t arg_time = 0); 
        
        // Grab available RF24 message
        bool Get_available_message();

        // Respond to sending NRF Node 
        void Respond_to_sending_node(uint8_t nrf_node = 0);
};
NRF24L01_radio RF24_radio;



// MQTT Client
class MQTT_client : public MQTTClient {
    public:
        // Begin
        bool Begin_object();

        // Connect to MQTT broker
        bool Connect();
        
        // Send NRF_Node data to DB (InfluxDB)
        void Send_node_data_to_db(uint8_t nrf_node); 
        
        // Grab available MQTT message 
        static void Get_message_and_respond(String &topic, String &payload);

        // Grab current time from Home Assistant
        void Get_current_time();
};
MQTT_client MQTT; 




//################  NRF24L01  ################

// Begin
bool NRF24L01_radio::Begin_object() {

    if (!(begin(&spiBus2, SPI_2_CE, SPI_2_CSN) && isChipConnected())) {
        return false; 
    } 

    // Config
    setPALevel(Radio_output);                                         // Transmitter strength   
    setChannel(Radio_channel);                                        // Radio channel (above wifi) 
    setDataRate(RF24_2MBPS);                                          // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
    openReadingPipe(This_dev_address, address[This_dev_address]);     // What pipe to listen on
    startListening(); 
    return true;
}


// Send message 
bool NRF24L01_radio::Send_message(uint16_t Local_RF24_package[6]) {
    println(__func__); 

    for (uint8_t i = 0; i < 6; i ++) { 
        // Insert Msg ID in the upper 4 bits
        uint16_t Payload = (i << 12) | Local_RF24_package[i];

        // Send message(s)
        if (!write(&Payload, sizeof(Payload))) {          
            for (uint8_t j = 0; j < 3; j ++) {          // Retry loop foreach [i]        
                delay(25);
                if(write(&Payload, sizeof(Payload))) {  // Exit retry loop if successful
                    break;                                      
                }
                else if (j == 2) {
                    return false;
                }  
            }
        }
        delay(5);      
    }
    return true;
}


// Try to send message (parameter hell)
bool NRF24L01_radio::Try_send_message(uint16_t arg_to_who, uint16_t arg_int, uint16_t arg_float, uint16_t arg_state, uint16_t arg_time) {
    println(); print(__func__);
    print(" to Node: "); println(arg_to_who);
    stopListening();
    openWritingPipe(address[arg_to_who]);

    // Create local copy of RF24_package[]  
    uint16_t Local_RF24_package[6]; 
    
    // Attiny84s have 2 bytes for Tx and Rx buffers...
    // splitting up package into small chunks of data                                
    Local_RF24_package[0] = arg_to_who;                // To who
    Local_RF24_package[1] = This_dev_address;          // From who
    Local_RF24_package[2] = arg_int;                   // "Int"
    Local_RF24_package[3] = arg_float;                 // "Float"
    Local_RF24_package[4] = arg_state;                 // "Bool"
    Local_RF24_package[5] = arg_time;                  // Time

    for (uint8_t i = 0; i < 10; i ++) {
        if (Send_message(Local_RF24_package)) {
            println("Message successful!");
            startListening(); 
            return true;
        } 
        delay(175); // Eventually falls inside a nodes "listening window"
    }
    println("Message failed!");
    return false;
} 


// Grab available RF24 message
bool NRF24L01_radio::Get_available_message() {
    println();
    println(__func__);
    unsigned long Msg_timer = millis() + 4 * 1000UL ;
    uint16_t First_node_ID;
    uint16_t Loop_node_ID;
    uint16_t Message;
        
    // Grab 6 packages in rapid succession
    for (uint8_t i = 0; i < 6; i ++) { 
        read(&Message, sizeof(Message));
        if (i == 0) {
            First_node_ID = (Message >> 12) & 0x0F;     // Get only Node_ID of the first message
            Loop_node_ID = First_node_ID;
        } 
        else {
            Loop_node_ID = (Message >> 12) & 0x0F;      // Get only Node_ID     
        }
        RF24_package[i] = Message & 0x0FFF;             // Get only message 
        flush_rx();

        // Node_ID didnt match! (2 Nodes sending at once)
        if (First_node_ID != Loop_node_ID) {
            println("Error! Node ID missmatch");
            return false;
        }
        // Wait for next RF24_package[i]. Break if time is up
        else if (i < 5) {                                    
            while (!available()) {
                if (millis() > Msg_timer) {
                    println("Error! Times up");
                    return false;
                }
            }  
        }       
    }
    return true; 
} 


// Respond to sending NRF Node 
void NRF24L01_radio::Respond_to_sending_node(uint8_t nrf_node) {
    println(__func__);
    print("Message from Node: ");
    println(nrf_node);
    uint16_t Current_time;

    // Convert current time into hhmm format
    if (year() > 2000) {
        Current_time = (hour() * 100 + minute());
    } else { 
        Current_time = 1200;
    }

    // Send to which node
    switch (nrf_node) {
        case 1: 
            // Respond to NRF_Node 1 with current time
            delay(5);
            if (Try_send_message(nrf_node, 0, 0, 0, Current_time)) {

                // Send Node data to InfluxDB
                MQTT.Send_node_data_to_db(nrf_node); 
            }
            break;
        case 2:
            // Placeholder 
            break;
        default:
            break;
    }
}



//###############  MQTT_client  ###############

// Begin
bool MQTT_client::Begin_object() {
    begin(Secret_MQTT_ip, 1883, ETH_client);
    onMessage(Get_message_and_respond);
    if (Connect()) { 
        return true;
    } else {
        return false; 
    } 
}


// Connect to MQTT broker
bool MQTT_client::Connect() {
    unsigned long Old_millis = millis();

    // Wait for connection
    while (!connect("Master_Node", Secret_MQTT_ssid, Secret_MQTT_password)) {
        print(".");
        delay(1000);
        if (millis() - Old_millis >= 15*1000UL) {
            return false;
        }
    }
    subscribe("/Give/Current_time");
    subscribe("/NRF_Node/1");
    subscribe("/NRF_Node/2");
    //unsubscribe("/hello");
    return true;
}


// Send NRF_Node data to DB
void MQTT_client::Send_node_data_to_db(uint8_t nrf_node) {
    println(__func__);
    bool Error_flag = false;

    // Json document
    StaticJsonDocument<128> Json_doc;
    String Payload;

    // Insert variables
    switch (nrf_node) {
        case 1: 
            Json_doc["Bat_percent"] = utils::Calc_remaining_battery_charge(); 
            break;
        case 2: 
            break;
        default:
            Error_flag = true;
            break;
    }
    if (!Error_flag) {
        serializeJson(Json_doc, Payload);
        String Topic = "/NRF_Node/" + String(nrf_node);
        publish(Topic.c_str(), Payload.c_str(), false, 2);
    }  
} 


// Grab available MQTT message with some logic sprinkled ontop //TODO
void MQTT_client::Get_message_and_respond(String &topic, String &payload) {
    //println();
    //print(__func__);
    //println(": " + topic + " - " + payload);

    // Get current time
    if (topic == "/Give/Current_time" && payload.length() == 10) {
        unsigned long Unix_timestamp = payload.toInt();
        setTime(Unix_timestamp);
        print("Time set: ");
        print(hour()); print(":"); print(minute()); print(":"); println(second());
        Prev_millis = millis();
        return;     
    }

    // Message to a RF24 Node
    else if ( (topic.substring(0, 10) == "/NRF_Node/") && (payload.length() >= 20 && payload.length() <= 40) ) {

        // Which node?
        String topic_substring = topic.substring(10, 11);
        uint8_t Which_node = topic_substring.toInt();

        // Parse JSON data + switch block
        StaticJsonDocument<128> jsonDoc;
        deserializeJson(jsonDoc, payload);

        switch (Which_node) {
            
            case 1: {         
                uint8_t Node_state = jsonDoc["Node_state"];
                uint8_t Pump_time = jsonDoc["Pump_time"];
                RF24_radio.Try_send_message(1, Pump_time, 0, Node_state, 0);
                break;
                } 

            case 2: {
                // Test //TODO
                if (Remove_this_var_TODO == 2) {
                    Remove_this_var_TODO = 3;
                }
                else if (Remove_this_var_TODO == 3) {
                    Remove_this_var_TODO = 2;
                }
                Transmit_RF_remote_codes(Remove_this_var_TODO);
                break;
                } 

            default:
                break;
        }
    }  
} 


// Grab current time from Home Assistant
void MQTT_client::Get_current_time() {
    println(__func__);
    publish("/Get/Current_time", "time", false, 2);
}  


