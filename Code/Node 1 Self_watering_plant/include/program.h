

// Program.h
#pragma once
#include "config.h"


// NRF24L01 Radio
class RF24Radio : public RF24 {        
    public:
        // Constructor (does inline even do anything?)
        inline RF24Radio(uint8_t ce = CE_PIN, uint8_t csn = CSN_PIN) : RF24(ce, csn) {};

        // Begin
        bool begin_object();

        // Send message 
        bool send_message(uint16_t* rf24_package_ptr); 

        // Try to send message
        bool try_send_message(bool request_time = false); 
        
        // Grab available message
        bool get_available_message(uint16_t* rf24_package_ptr);

        // Wait for incomming message 
        bool wait_for_message(uint16_t how_long, uint16_t* rf24_package_ptr);

        // Send battery and waterlevel status, get back the current time
        void send_data_get_time();

    private:
        // Radio device addresses {Master, self}    
        const uint8_t address[2][6] = {"0Adrs", RF24_THIS_DEV_ADDRESS_STRING};  
};



// Hardware 
class HardwareClass {
    public:
        // Constructor
        HardwareClass() {
            sleep_at_this_timestamp = 0;            
            send_data_timestamp = 0; 
        };
        
        // Setup 
        void setup();

        // Get ADC reading from battery circuit, calculations done on master 
        uint16_t measure_battery_charge();

        // Measure current water level
        uint8_t measure_water_level();

        // Start or do nothing? 
        void water_pump_control(uint16_t* rf24_package_ptr);

        // Start waterpump for arg seconds
        void run_water_pump(uint8_t how_long = 2);

        // Hardware reset
        void reset_devices();

        // ATtiny84 + NRF24 deepsleep (Total board consumption about 9 µA)
        #if ATTINY84_ENABLED
            void deepsleep(bool only_15_minutes = false);
        #endif
    
        // public properties
        uint32_t sleep_at_this_timestamp;            
        uint32_t send_data_timestamp; 
};



namespace helper {

// Used <TimeLib.h> before, but ran out of progmem :/
void calc_time_until_deepsleep(uint16_t rf24_package_time);

// Watchdog interrupt service routine (Deepsleep)
#if ATTINY84_ENABLED 
    ISR (WDT_vect);
#endif

} // namespace helper



// ####################  DEBUG / CALIBRATION  ####################

// Battery ADC calibration
#if SERIAL_ENABLED && ADC_CAL_ENABLED
    void adc_calibration();

// Pump calibration
#elif SERIAL_ENABLED && PUMP_CAL_ENABLED
    void pump_calibration();

// Print message package
#elif SERIAL_ENABLED && !ATTINY84_ENABLED
    void print_package(uint16_t* rf24_package);
#endif


