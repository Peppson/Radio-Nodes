
/*
#######################  Functions.h  #######################
*/


#pragma once
#include "Config.h"
#include "Classes.h"


// Transmit 433Mhz. Buttons: 1_on, 1_off, 2_on, 2_off, 3_on, 3_off, All_off
void Transmit_RF_remote_codes(uint8_t button, uint8_t retries) {
    println(__func__);

    // Number of retries
    for (uint i = 0; i < retries; i ++) {

        // Loop through the choosen "button code" array, 2 steps at the time
        for (uint8_t j = 0; j < 131; j += 2) {            
            uint16_t High_duration = Secret_RF_remote_codes[button][j];
            uint16_t Low_duration = Secret_RF_remote_codes[button][j + 1];

            // High pulse
            digitalWrite(RF433_TX_PIN, HIGH);
            delayMicroseconds(High_duration);

            // Low pulse
            digitalWrite(RF433_TX_PIN, LOW);
            delayMicroseconds(Low_duration);
        }
        delay(1); //TODO?
    }
}


namespace utils {

// Calculate a NRF_nodes remaining battery charge //TODO
uint16_t Calc_remaining_battery_charge() {
    println(__func__);
    // Li-ion batteries do not discharge linearly!
    // https://www.icode.com/how-to-measure-and-display-battery-level-on-micro-controller-arduino-projects/
    
    print("Node ADC value: ");
    println(RF24_package[3]);

    // Calculate max 4.2v min? 
    /*
    uint8_t Value = RF24_package[3];
    if (Value >= 2650) Bat_percent = 100;
    else if (Value >= 2500)    // 85-100% range
        Bat_percent = map(Value, 2500, 2650, 85, 100);
    else if (Value >= 2100)    // 10-85% range
        Bat_percent = map(Value, 2100, 2500, 10, 85);
    else if (Value >= 1700)    // 0-10% range
        Bat_percent = map(Value, 1700, 2100, 0, 10);
    else Bat_percent = 0; */


    //return Bat_percent;
    return 42;
}


// Ethernet debug
void WiFiEvent(WiFiEvent_t event) {
    // Copy pasta
    switch (event) {
        case SYSTEM_EVENT_ETH_START:
            println("ETH Started");
            ETH.setHostname("esp32-master"); // Set eth hostname here 
            break;
        case SYSTEM_EVENT_ETH_CONNECTED:
            println("ETH Connected");
            break;
        case SYSTEM_EVENT_ETH_GOT_IP:
            print("ETH MAC: ");
            print(ETH.macAddress());
            print(", IPv4: ");
            print(ETH.localIP());
            if (ETH.fullDuplex()) {
                print(", FULL_DUPLEX");
            }
            print(", ");
            print(ETH.linkSpeed());
            println("Mbps");
            break;
        case SYSTEM_EVENT_ETH_DISCONNECTED:
            println("ETH Disconnected");
            break;
        case SYSTEM_EVENT_ETH_STOP:
            println("ETH Stopped");
            break;
        default:
            break;
    }
}


// Test internet connection
bool Test_ETH_connection(const char * host, uint16_t port) {
    print("Connecting to ");
    println(host);

    if (!ETH_client.connect(host, port)) {
        println("Connection failed");
        return false;
    }
    ETH_client.printf("GET / HTTP/1.1\r\nHost: %s\r\n\r\n", host);
    while (ETH_client.connected() && !ETH_client.available());
        while (ETH_client.available()) {
            Serial.write(ETH_client.read());
    }
    ETH_client.stop();
    return true;
}


// Test hardware
void Test_connected_hardware() {

    // Ethernet
    if (utils::Test_ETH_connection("192.168.1.1", 80)) {
        println("Router \tOK!");    
    } else {
        println("Router \tOffline!"); 
    }
    // NRF24L01 radio
    if (RF24_radio.Begin_object()) {
        println("RF24 \tOK!");    
    } else {
        println("RF24 \tOffline!"); 
    }
    // MQTT client
    if (MQTT.Begin_object()) {
        println("MQTT \tOK!");      
    } else {
        println("MQTT \tOffline!");  
    }
}


// Setup
void Setup_hardware() {

    // Serial
    #if SERIAL_ON
        Serial.begin(9600);
        while (!Serial) {
        } 
        // Fill console with \n 
        for(uint8_t i=0; i<25; i++) {
            println();
        }
    #endif

    // SPI buses
    spiBus1.begin();
    spiBus2.begin(SPI_2_SCLK, SPI_2_MISO, SPI_2_MOSI, SPI_2_CE);

    // Ethernet
    WiFi.onEvent(WiFiEvent);
    ETH.begin(ETH_ADDR, ETH_POWER_PIN, ETH_MDC_PIN, ETH_MDIO_PIN, ETH_TYPE, ETH_CLK_MODE);
    ETH.config(IPAddress(192, 168, 1, 25), IPAddress(192, 168, 1, 1), IPAddress(255, 255, 255, 0));
    WiFi.mode(WIFI_OFF);

    // Tests
    Test_connected_hardware();

    // 433Mhz radio
    pinMode(RF433_TX_PIN, OUTPUT);  // TX pin
    pinMode(RF433_RX_PIN, INPUT);   // RX pin

    // Allow things to settle down 
    delay(3000);
}


//######  DEBUG  ######
#if SERIAL_ON
void Print_package() {
    println();
    println("#########  Package  #########");
    print("To who:      "); println(RF24_package[0]);
    print("From who:    "); println(RF24_package[1]);
    print("Int:         "); println(RF24_package[2]);
    print("Float:       "); println(RF24_package[3]);
    print("Bool:        "); println(RF24_package[4]);
    print("Time:        "); println(RF24_package[5]);
    println();
}
#endif

}


