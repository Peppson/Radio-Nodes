
#pragma once
namespace selected_node {


// Node specific configs
constexpr static uint32_t SLEEP_DURATION_SECONDS = 12*60*60UL;       // Deepsleep duration in seconds
constexpr static uint32_t SLEEP_AT_THIS_TIME = 2100UL;               // Sleep at what time? (hhmm)
constexpr static uint32_t WATER_SENSOR_MIN_VALUE = 65UL;             // Calibrated value with EMPTY tank
constexpr static uint32_t WATER_SENSOR_MAX_VALUE = 270UL;            // Calibrated value with FULL tank
constexpr static uint32_t NODE_UPDATE_INTERVAL = 8*60*60*1000UL;     // How often should the node send status update to master? (ms)
}


#define COFFEE_COUNT 0                                // Starting coffee_count value, needs to be set at uploading (wipes prev value at upload)
#define COFFEE_ADC_THRESHOLD 0                        // Current sensor threshold 
#define SERVO_ON_POS 0                                // On position of servo
#define SERVO_ON_MIDDLE_POS 0                         // Middle position after ON of servo
#define SERVO_OFF_POS 0                              // Off position of servo
#define SERVO_OFF_MIDDLE_POS 0                        // Middle position after OFF of servo