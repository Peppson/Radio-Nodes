
#pragma once
#include "../global_config.h"
class RF24Radio;

// RTC Interrupt handler. Wakes up the CPU periodically while deepsleeping
ISR (RTC_CNT_vect);


class Hardware {
public:
    // Used if battery measurement is needed
    Hardware(uint8_t ADC_enable_pin, uint8_t ADC_measure_pin) {
        pin_ADC_enable = ADC_enable_pin;    
        pin_ADC_measure = ADC_measure_pin;
    };

    // Default 
    Hardware() {
        pin_ADC_enable = 0;    
        pin_ADC_measure = 0;
    };

    static void begin(RF24Radio* radio_ptr);
    static void init_USB_serial();
    static void restart_device();
    static void deepsleep(uint32_t sleep_duration_s);
    static void lightsleep(uint16_t sleep_duration_ms);
    static void disable_all_peripherals();
    static void timeout(uint32_t timeout_ms);
    static uint16_t get_remaining_battery_charge();
    static bool set_time_until_deepsleep(uint16_t time_input);
    static inline bool verify_time_input(const uint16_t time);

    static uint32_t deepsleep_timestamp; 
    static uint32_t send_data_timestamp;
    static uint8_t pin_ADC_enable;
    static uint8_t pin_ADC_measure;  

private:
    static RF24Radio* _radio_ptr;
};
