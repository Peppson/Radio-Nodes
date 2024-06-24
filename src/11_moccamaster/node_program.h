/*


void move_servo(uint16_t position, bool on_boot) {

    // Check coffe_state only on boot, move servo accordingly
    if (on_boot) {
        position = (hardware.coffee_state) ? SERVO_ON_MIDDLE_POS : SERVO_OFF_MIDDLE_POS;
    }
    // Set high and low pulses
    uint16_t high_pulse = position;
    uint16_t low_pulse = 20*1000 - high_pulse;

    // Emulate PWM signal
    for (uint8_t i = 0; i < 20; i++) {
        digitalWrite(SERVO_PIN, HIGH); 
        delayMicroseconds(high_pulse); 
        digitalWrite(SERVO_PIN, LOW); 
        delayMicroseconds(low_pulse); 
    }
    // Move servo back to either ON/OFF middle positions, if moved elsewhere
    if (position == SERVO_ON_POS) { 
        move_servo(SERVO_ON_MIDDLE_POS); 
    } else if (position == SERVO_OFF_POS) { 
        move_servo(SERVO_OFF_MIDDLE_POS);
    }   
}





*/



#if 0


bool HardwareClass::get_mocca_master_state() { 
    WDT_RESET();
    
    // Enable ADC
    ADCSRA |= (1 << ADEN);

    // Grab ADC readings (0-1023)
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 100; i++) {
        sum += analogRead(ADC_MEASURE_PIN);
        delay(1); 
    }
    // Disable ADC
    ADCSRA &= ~(1 << ADEN); 

    return (sum / 100 > COFFEE_ADC_THRESHOLD) ? true : false;                
}



void HardwareClass::mocca_master_control(uint16_t* rf24_package_ptr) {
    WDT_RESET();
    uint16_t from_who = rf24_package_ptr[0];
    uint16_t to_who = rf24_package_ptr[1];
    bool coffe_control = static_cast<bool>(rf24_package_ptr[4]);
    uint16_t reboot = rf24_package_ptr[5];  
     
    // Message to this dev?
    if ((from_who == RF24_MASTER_NODE_ADDRESS) && (to_who == RF24_THIS_DEV_ADDRESS)) {
        
        // Reboot
        if (reboot == 2222) {
            hardware.reset_devices();
        }
        // Start or stop
        if (coffe_control) {
            move_servo(SERVO_ON_POS);
        } else if (!coffe_control) {
            move_servo(SERVO_OFF_POS);
        } 
    }
}
#endif