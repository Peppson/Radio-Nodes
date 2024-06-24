
#pragma once
#include "../global_config.h"


class WaterSystem {
public:
    WaterSystem(uint8_t pin_out, uint8_t pin_in, uint16_t max_value, uint16_t min_value) {
        _pin_out = pin_out;
        _pin_in = pin_in;
        _sensor_max_value = max_value;
        _sensor_min_value = min_value;
    };

    static uint8_t get_water_level();
    static void run_water_pump(uint16_t duration_seconds);

private:
    static uint8_t _pin_out;
    static uint8_t _pin_in;
    static uint16_t _sensor_max_value;
    static uint16_t _sensor_min_value;
};
