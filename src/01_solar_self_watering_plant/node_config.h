
#pragma once
namespace selected_node {


// Node specific configs
constexpr static uint32_t SLEEP_DURATION_SECONDS = 12*60*60UL;       // Deepsleep duration in seconds
constexpr static uint32_t SLEEP_AT_THIS_TIME = 2100UL;               // Sleep at what time? (hhmm)
constexpr static uint32_t WATER_SENSOR_MIN_VALUE = 70UL;             // Calibrated value with EMPTY tank
constexpr static uint32_t WATER_SENSOR_MAX_VALUE = 265UL;            // Calibrated value with FULL tank
constexpr static uint32_t NODE_UPDATE_INTERVAL = 8*60*60*1000UL;     // How often should the node send status update to master? (ms)
}

// Water tank = 7dL 