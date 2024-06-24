
#include "water_system.h"
#include "capacitor_lite/capacitor_lite.h"


// Members
uint8_t WaterSystem::_pin_out;
uint8_t WaterSystem::_pin_in;
uint16_t WaterSystem::_sensor_max_value;
uint16_t WaterSystem::_sensor_min_value;


uint8_t WaterSystem::get_water_level() {
    CapacitorLite water_sensor(_pin_out, _pin_in);
    constexpr uint8_t samples = 100;

    // Enable ADC and feed the hungry Watchdog
    ADC0.CTRLA |= ADC_ENABLE_bm; 
    WDT_FEED();

    // Sum ADC readings (0-1023)
    uint32_t sum = 0;
    for (size_t i = 0; i < samples; i++) {
        uint16_t level = water_sensor.measure(); 
        sum += constrain(level, 0, 1023);
        delay(1);
    }           

    // Disable ADC
    ADC0.CTRLA &= ~ADC_ENABLE_bm; 

    // Return water level
    uint32_t average = sum / samples;
    if (average <= _sensor_min_value) {
        return 0;
    } else {
        // Actually very linear, see images/Node_1_water_sensor_example.PNG
        return map(average, _sensor_min_value, _sensor_max_value, 0, 100);
    }
}


void WaterSystem::run_water_pump(uint16_t duration_seconds) {
    // In case of error...
    if (duration_seconds > WATER_PUMP_RUNTIME_MAX) {
        duration_seconds = 1;
    }

    // Start pump with N-channel mosfet
    uint32_t end_time = (duration_seconds * 1000) + millis();
    digitalWrite(PIN_POWER_IO, HIGH);

    // Monitor water level while pumping
    while (millis() < end_time) {
        WDT_FEED();
        delay(5);

        // Stop if watertank is empty
        /* if (!get_water_level()) { //TODO
            break; 
        } */ 
    }
    
    // Turn off pump
    digitalWrite(PIN_POWER_IO, LOW); 
}
