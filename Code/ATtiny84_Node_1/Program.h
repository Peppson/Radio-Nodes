


// Program.h
#ifndef __PROGRAM_H__
#define __PROGRAM_H__
#include "Config.h"


// NRF24L01 Radio
class NRF24L01_radio : public RF24 {
    private:
        // Radio device addresses {Master, self}    
        const uint8_t address[2][6] = {"0Adrs", RF24_THIS_DEV_ADDRESS_STR};  
        
    public:
        // Constructor inline
        NRF24L01_radio(uint8_t CE = CE_PIN, uint8_t CSN = CSN_PIN) : RF24(CE, CSN) {};

        // Begin
        bool Begin_object();

        // Send message 
        bool Send_message(uint16_t* RF24_package_ptr); 

        // Try to send message
        bool Try_send_message(/*uint16_t arg_to_who = 0, uint16_t arg_int = 0, uint16_t arg_float = 0, uint16_t arg_state = 0, uint16_t arg_time = 0*/); 
        
        // Grab available message
        bool Get_available_message(uint16_t* RF24_package_ptr);

        // Wait for incomming message 
        bool Wait_for_message(uint16_t how_long, uint16_t* RF24_package_ptr);

        // Send battery and waterlevel status, get back the current time
        void Send_data_get_time();
};



// Hardware 
class Hardware_class {
    public:
        // Setup 
        void Setup();

        // Get ADC reading from battery circuit
        uint16_t Battery_charge_remaining();

        // // Start waterpump for param seconds
        void Start_water_pump(uint8_t How_long = 2);

        // Hardware reset
        void Reset_devices(bool now);

        // Check watertank
        bool Is_watertank_empty();

        // ATtiny84 + NRF24 deepsleep (Total board cunsumption about 9 µA)
        #if __ATTINY84_ON__
            void Deepsleep();
        #endif
};



namespace helper {

// Read in values from EEPROM memory
uint8_t Read_waterlevel_from_EEPROM(bool calc = false);

// Used to have <TimeLib.h> for this but ran out of progmem :/
void Calc_time_until_sleep(uint16_t* RF24_package_ptr);

// Deepsleep Watchdog interrupt service routine
#if __ATTINY84_ON__ 
    ISR (WDT_vect);
#endif

} // namespace helper



//####################  DEBUG  ####################

// ADC/volt calibration 
#if __SERIAL_ON__ && __ADC_CAL_ON__
    void ADC_CAL_FUNC();

// Print message package
#elif __SERIAL_ON__ && !__ATTINY84_ON__
    void Print_package(uint16_t* RF24_package);
#endif


#endif // __PROGRAM_H__


