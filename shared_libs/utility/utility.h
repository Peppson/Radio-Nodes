
#pragma once 
#include "../global_config.h"


namespace util {
    void benchmark(const char* name = nullptr);
    void print_int_hack(const uint32_t& value);
    void calibration();
    void adc_calibration();
    void pump_calibration();
    void servo_calibration();
    void button_halt_CPU();
    void debug_loop();
}
