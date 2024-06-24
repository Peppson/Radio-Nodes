
/*
  // Trimmed down, full version @
  
  CapacitorLite.h - Library for measuring capacitance
  Created by Jon Nethercott @ Codewrite, 28 May 2018
  Released into the public domain under the GNU GPLv3
  https://github.com/codewrite/arduino-capacitor
*/

#pragma once
#include "../global_config.h"

// Stray capacitance and Pullup resistance will vary depending on board -
// For maximum accuracy calibrate with known capacitor.
class CapacitorLite {
    public:
        // Constructor  
        CapacitorLite(uint8_t outPin, uint8_t inPin);

        // Measure capacitance
        uint32_t measure();         

    private:
        uint16_t _maxAdcValue = 1023;
        static unsigned int _inCapToGnd;
        uint8_t _outPin;
        uint8_t _inPin;
};


