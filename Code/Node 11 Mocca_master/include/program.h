

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
            coffee_state = false;
            prevoius_timestamp = 0;
            sleep_at_this_timestamp = 0;            
        };
   
        // Setup 
        void setup();

        // Get ADC reading from current sensor. Mocca master on or off?
        bool get_mocca_master_state();

        // Start, stop or do nothing?
        void mocca_master_control(uint16_t* rf24_package_ptr);

        // Generate 50Hz software PWM signal (20ms period) (1/50Hz)
        void move_servo(uint16_t position, bool on_boot = false);

        // Read/Write EEPROM memory. Logging numbers of coffee made
        uint16_t read_value_from_EEPROM();
        void update_value_to_EEPROM();
        
        // EEPROM memory checks on boot. Uninitialized memory = 255
        void check_EEPROM_on_boot();

        // Hardware reset
        void reset_devices();

        // ATtiny84 + NRF24 deepsleep (Total board consumption about 9 µA)
        #if ATTINY84_ENABLED
            void deepsleep(bool only_15_minutes = false);
        #endif
    
        // public properties
        bool coffee_state; 
        uint16_t coffee_count;
        uint32_t prevoius_timestamp;
        uint32_t sleep_at_this_timestamp;                   
};



namespace helper {

// Used <TimeLib.h> before, but ran out of memory :/
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

// Print message package
#elif SERIAL_ENABLED && !ATTINY84_ENABLED
    void print_package(uint16_t* rf24_package);
#endif


