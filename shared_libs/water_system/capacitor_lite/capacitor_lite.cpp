
/*
  // Trimmed down, full version @
  
  Capacitor.cpp - Library for measuring capacitance
  Created by Jon Nethercott @ Codewrite, 28 May 2018
  Released into the public domain under the GNU GPLv3
  https://github.com/codewrite/arduino-capacitor
*/

#include "capacitor_lite.h"
#include "limits.h"
    
#define STRAY_CAP (2448);
unsigned int CapacitorLite::_inCapToGnd = STRAY_CAP; // In pF *100


// Constructor
CapacitorLite::CapacitorLite(uint8_t outPin, uint8_t inPin) {
    _outPin = outPin;
    _inPin = inPin;
    digitalWrite(_outPin, LOW);
    digitalWrite(_inPin, LOW);
    pinMode(_outPin, OUTPUT);
    pinMode(_inPin, OUTPUT);
}


// Capacitor under test between _outPin and _inPin
// Returns capacitance in pF * 100
uint32_t CapacitorLite::measure() {
    long capacitance;

    pinMode(_inPin, INPUT);                 
    digitalWrite(_outPin, HIGH);
    int val = analogRead(_inPin);
    digitalWrite(_outPin, LOW);
    pinMode(_inPin, OUTPUT);   
              
    // Calculate result
    capacitance = (long)val * _inCapToGnd / (max(_maxAdcValue - val, 1));
    return (unsigned int)min(capacitance, UINT_MAX);
}


