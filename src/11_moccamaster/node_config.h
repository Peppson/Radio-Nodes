
#pragma once
#include "../global_config.h"


// Node specific configs
constexpr uint32_t UPDATE_INTERVAL = 3*60*60*1000;      // How often should the node send status update to master? (h*m*s*ms)
#define DEEPSLEEP_AT_THIS_TIME 2100                         // Go to deepsleep at what time? (hhmm)
#define DEEPSLEEP_HOW_MANY_HOURS 12                         // Deepsleep how long in (h)

#define COFFEE_COUNT 290                                // Starting coffee_count value, needs to be set at uploading (wipes prev value at upload)
#define COFFEE_ADC_THRESHOLD 100                        // Current sensor threshold 
#define SERVO_ON_POS 550                                // On position of servo
#define SERVO_ON_MIDDLE_POS 800                         // Middle position after ON of servo
#define SERVO_OFF_POS 1300                              // Off position of servo
#define SERVO_OFF_MIDDLE_POS 950                        // Middle position after OFF of servo

/* 
Servo postitons:
On = 550
On_middle = 800
Off = 1300
Off_middle = 950 
*/
