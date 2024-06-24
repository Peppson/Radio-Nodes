
#include "utility.h"
#include "hardware.h"
#include "water_system.h"


namespace util {


// Timer function. Call before and after measured code
void benchmark(const char* name) {
    static bool begin_timer = true;
    static uint32_t start_time = 0;

    // Start timer
	if (begin_timer) {
        begin_timer = false;
		start_time = micros();
        return;
	}

    // Stop timer + output
    uint32_t end_time = micros() - start_time;
    String num_str = String(end_time);

    // Name (if any)
    if (name != nullptr) { log("%s: ", name); }
    
    // Microseconds
    if (num_str.length() < 4) {
        log("%iμs \n", end_time);

    // Milliseconds
    } else if (num_str.length() < 6) {
        float end_ms = (float)end_time / 1000;
        log("%.3fms \n", end_ms);
    } else {
        log("%ims \n", end_time / 1000);
    }

    // Reset
    begin_timer = true;
    start_time = 0;	
}


// Attinys can only print upto an int16_t, this is some kind of workaround for larger nums
void print_int_hack(const uint32_t& value) {
    #if USB_SERIAL_ENABLED
        char buffer[20];
        sprintf(buffer, "%lu", value);
        log("%s\n", buffer);
        delay(20);
    #endif
}


void calibration() {
    WDT_DISABLE(); 

    #if !USB_SERIAL_ENABLED && CALIBRATION
        #error "--- Enable USB_SERIAL_ENABLED in \"global_config.h\" for util::calibration() ---"
    #endif

    #if DEBUG_LOOP_ENABLED
        log("DEBUG_LOOP_ENABLED\n"); 
        debug_loop();
    #elif ADC_CAL_ENABLED
        log("ADC_CAL_ENABLED\n");
        adc_calibration();
    #elif PUMP_CAL_ENABLED
        log("PUMP_CAL_ENABLED\n"); 
        pump_calibration();
    #elif SERVO_CAL_ENABLED
        log("SERVO_CAL_ENABLED\n");
        servo_calibration();
    #endif
}


void adc_calibration() { 
    ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
    digitalWrite(Hardware::pin_ADC_enable, HIGH);

    log("GOOOO!!!\n");

    while (1) {
        uint32_t sum = 0;
        for (uint8_t i = 0; i < 100; i++) {
            uint16_t level = analogRead(Hardware::pin_ADC_measure); 
            sum += constrain(level, 0, 1023);
            delay(1); 
        }
        
        uint16_t average = sum / 100;           
        log("Avg: %i\n", average);
        delay(500);
    }
}


void pump_calibration() {
    ADC0.CTRLA |= ADC_ENABLE_bm; // Enable ADC
    //digitalWrite(PIN_POWER_IO, HIGH); // Run pump
    
    while (1) {
        uint32_t sum = 0;
        for (uint8_t i = 0; i < 100; i++) {
            uint16_t value = WaterSystem::get_water_level();
            sum += constrain(value, selected_node::WATER_SENSOR_MIN_VALUE, selected_node::WATER_SENSOR_MAX_VALUE);
        }

        uint32_t level = sum / 100;
        uint32_t percent = map(level, selected_node::WATER_SENSOR_MIN_VALUE, selected_node::WATER_SENSOR_MAX_VALUE, 0, 100);
        log("Average sum: %i\n", level);
        log("Map percent: %i\n", percent);
        delay(500); 
    }
}


void servo_calibration() {
    while (1){
        delay(1);
    }
    
}


void button_halt_CPU() {
    pinMode(PIN_PA4, INPUT_PULLUP);
    pinMode(PIN_UPDI, INPUT_PULLUP);

    if (digitalRead(PIN_PA4) == LOW) {
        log("CPU halt!\n");
        while (1) {
            delay(1000);
        }
    }
}


void debug_loop() {
    while (1) {
        delay(100);
    }
}
} // Namepsace util
