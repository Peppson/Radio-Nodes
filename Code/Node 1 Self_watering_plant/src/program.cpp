

// program.cpp
#include "program.h"
#include "config.h"


// Global counter while in deepsleep
volatile int deepsleep_counter = 0;   

// Objects
extern RF24Radio radio_24;
extern HardwareClass hardware;
extern CapacitorLite water_sensor;  
#if ATTINY84_ENABLED && SERIAL_ENABLED
    SendOnlySoftwareSerial tiny_serial(SERIAL_OUT_PIN);
#endif



// ##################  RF24  ##################

// Begin 
bool RF24Radio::begin_object() {

    // Init
    if (!begin() && !isChipConnected()) {
        println("RF24 Offline!"); 
        return false; 
    }
    // Config
    setPALevel(RF24_OUTPUT);                                // Transmitter strength   
    setChannel(RF24_CHANNEL);                               // Radio channel 
    setDataRate(RF24_DATARATE);                             // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
    openWritingPipe(address[RF24_MASTER_NODE_ADDRESS]);     // Send to which address
    openReadingPipe(RF24_PIPE, address[1]);                 // Listen on which address
    #if SERIAL_ENABLED
        println("RF24 Online!");
        printDetails();
    #endif
    return true;
}


// Send message 
bool RF24Radio::send_message(uint16_t* rf24_package_ptr) {
    for (uint8_t i = 0; i < 6; i++) {
        
        // Insert Msg ID in the upper most 4 bits
        uint16_t payload = (RF24_THIS_DEV_ADDRESS << 12) | rf24_package_ptr[i];  

        // Send message(s)
        if (!write(&payload, sizeof(payload))) {

            // Retry loop foreach index
            for (uint8_t j = 0; j < 3; j++) {                                
                delay(15);

                // Exit retry loop if successful
                if (write(&payload, sizeof(payload))) {     
                    break;
                // Failed                                       
                } else if (j == 2) {                                          
                    return false;
                }  
            }
        }
        delay(15); // Important!
    }
    return true;
} 


// Try to send message 
bool RF24Radio::try_send_message(bool request_time) {
    stopListening();
    
    // Create local message array 
    uint16_t rf24_package[6] = {}; 
    
    // Attiny84s (this device) have 2 bytes for Tx and Rx buffers...
    // splitting up package into smaller chunks                 
    rf24_package[0] = RF24_THIS_DEV_ADDRESS;                // From who
    rf24_package[1] = RF24_MASTER_NODE_ADDRESS;             // To who
    rf24_package[2] = hardware.measure_water_level();       // Current waterlevel calculate to % (1-100) 
    rf24_package[3] = hardware.measure_battery_charge();    // Battery charge remaining (ADC reading 0-1023)
    rf24_package[4] = 0;                                    // 
    rf24_package[5] = request_time;                         // Request current time or just update 

    // Send message
    for (uint8_t i = 0; i < 4; i++) {
        if (send_message(rf24_package)) {
            debug_println("Message OK!");
            return true;
        }
        delay(400);
    }
    debug_println("Message Failed!");
    return false;
}


// Grab available RF24 message
bool RF24Radio::get_available_message(uint16_t* rf24_package_ptr) {
    uint32_t timer = millis() + 1500UL;
    uint16_t payload = 0;
    uint8_t msg_id = 0;
    uint8_t msg_sum = 0;
    
    // Grab 6 packages in quick succession
    for (uint8_t i = 0; i < 6; i++) {
        read(&payload, sizeof(payload));
        msg_id = (payload >> 12) & 0x0F;                // "Decode" message, save only Msg ID
        
        // Append array only if msg_id = i 
        if (msg_id == i) {
            rf24_package_ptr[i] = payload & 0x0FFF;     // "Decode" message, save only Msg
            msg_sum += msg_id;
        } else {
            i--;
        }

        // Wait for next Message. Return false if time runs out
        while (i < 5 && !available()) {                                        
            if (millis() > timer) {
                debug_println("Error! Times up");
                return false;
            }
        }
    }
    // Expected "Checksum" ? 0+1+2...+5
    return (msg_sum == 15) ? true : false;
}


// Wait for incomming RF24 message 
bool RF24Radio::wait_for_message(uint16_t how_long, uint16_t* rf24_package_ptr) {
    uint32_t timer = (millis() + how_long);
    uint8_t pipe;
    WDT_RESET();

    // Start listening
    while (1) {
        if (available(&pipe) && pipe == RF24_PIPE) {
            return get_available_message(rf24_package_ptr);
        }
        // Break after how_long milliseconds 
        if (millis() > timer) {
            return false;
        }
    }
}


// Send node data, get back the current time
void RF24Radio::send_data_get_time() {
    WDT_RESET(); 
    
    // Ask master for time 
    try_send_message(true);
    startListening();
    uint16_t rf24_package[6] = {};
    uint8_t loop_counter = 0;
    bool error_flag = true;
    
    // Wait for response from master for up to 6 seconds, max 4 iterations
    do {
        if (wait_for_message(6000, rf24_package)     
            && ((rf24_package[0] == RF24_MASTER_NODE_ADDRESS) 
            &&  (rf24_package[1] == RF24_THIS_DEV_ADDRESS))) {

            // Code from master "Resend message"
            if (rf24_package[5] == 3333) {
                try_send_message(true);
                loop_counter--;
            // Current time from master
            } else {
                helper::calc_time_until_deepsleep(rf24_package[5]);
                error_flag = false;   
            }   
        }
        loop_counter++;
    } while (error_flag && loop_counter < 4); 
    
    // Still no answer? Deepsleep for 15mins and try again
    if (error_flag) { DEEPSLEEP(true); }
}



// ##################  Hardware  ##################

// Setup 
void HardwareClass::setup() {

    // Reset Watchdog on boot
    #if ATTINY84_ENABLED
        MCUSR &= ~(1<<WDRF);
        WDT_DISABLE();
    #endif

    // Allow things to settle down
    delay(5000);
    
    // Serial
    #if SERIAL_ENABLED
        SERIAL_BEGIN(9600);
        while (!Serial) {}
        println(); 
    #endif

    // Physical I/O
    pinMode(ADC_MEASURE_PIN, INPUT);
    pinMode(ADC_ENABLE_PIN, OUTPUT);
    pinMode(PUMP_PIN, OUTPUT);
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(ADC_ENABLE_PIN, LOW);
    
    // Disable Analog to Digital Converter, save power
    ADCSRA &= ~(1 << ADEN);

    // Begin RF24 radio and Watchdog
    radio_24.begin_object();
    #if WATCHDOG_ENABLED                                            
        WDT_ENABLE(WDTO_8S);
    #endif  
}


// Get ADC reading from battery circuit
uint16_t HardwareClass::measure_battery_charge() { 
    WDT_RESET();
    radio_24.stopListening();
    radio_24.powerDown();
    delay(5);
    
    // Enable the ADC and toggle transistor pair to allow current to ADC
    ADCSRA |= (1 << ADEN);
    digitalWrite(ADC_ENABLE_PIN, HIGH);

    // Grab ADC readings (0-1023)
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 100; i++) {
        sum += analogRead(ADC_MEASURE_PIN);
        delay(10); 
    }
    // Average ADC value (0-1023), all calculations are done on master to save progmem
    digitalWrite(ADC_ENABLE_PIN, LOW);
    ADCSRA &= ~(1 << ADEN); // Disable ADC
    radio_24.powerUp();
    delay(10);

    #if ATTINY84_ENABLED
        return sum / 100;                
    #else
        return 42;
    #endif
}


// Measure water level with DIY capacitive sensor
uint8_t HardwareClass::measure_water_level() {
    WDT_RESET();
    ADCSRA |= (1 << ADEN); // Enable ADC

    // Grab ADC readings (0-1023)
    uint32_t sum = 0;
    for (uint8_t i = 0; i < 100; i++) {
        uint16_t value = water_sensor.measure();
        value = constrain(value, 0, WATER_SENSOR_MAX_VALUE);
        sum += value;
    }           
    ADCSRA &= ~(1 << ADEN); // Disable ADC

    // Calculate waterlevel %
    uint32_t average = sum / 100;

    // Actually really linear sensor, see Images/Node 1 water_sensor_calibration.PNG
    return (average <= WATER_SENSOR_MIN_VALUE) ? 0 : map(average, WATER_SENSOR_MIN_VALUE, WATER_SENSOR_MAX_VALUE, 0, 100); 
}


// Start or do nothing? 
void HardwareClass::water_pump_control(uint16_t* rf24_package_ptr) {
    WDT_RESET();
    uint16_t from_who = rf24_package_ptr[0];
    uint16_t to_who = rf24_package_ptr[1];
    bool should_start = static_cast<bool>(rf24_package_ptr[4]);

    if ((from_who == RF24_MASTER_NODE_ADDRESS) 
        && (to_who == RF24_THIS_DEV_ADDRESS) 
        && (should_start)) {
        hardware.run_water_pump(rf24_package_ptr[2]);
    } 
}


// Run water pump for arg seconds
void HardwareClass::run_water_pump(uint8_t how_long) {
    radio_24.stopListening();
    
    // In case of error...
    if (how_long >= PUMP_RUNTIME_MAX) {
        how_long = 1;
    }
    // Toggle 2N2222A transistor with PUMP_PIN
    digitalWrite(PUMP_PIN, HIGH);
    delay(100); // Water pump inrush current
    uint32_t timer = (how_long * 1000) + millis();
    uint8_t current_water_level;

    // Monitor waterlevel while pumping
    while (millis() < timer) {
        delay(5);
        current_water_level = measure_water_level();

        // Stop if watertank is empty
        if (!current_water_level) {
            break;
        } 
    }
    // Waterpump can draw a lot of current on startup 
    // RF24 needs software reset sometimes...
    digitalWrite(PUMP_PIN, LOW);
    delay(10); 
    radio_24.begin_object();     
}


// Hardware reset
void HardwareClass::reset_devices() { 

    // Wait for watchdog to reset board
    #if ATTINY84_ENABLED
        WDT_DISABLE();
        WDT_ENABLE(WDTO_15MS);
        delay(1000); 
    #endif
}


// ATtiny84 + NRF24L01+ deepsleep (Total board cunsumption about 10 µA)
#if ATTINY84_ENABLED
    void HardwareClass::deepsleep(bool only_15_minutes) {
        int deepsleep_target;

        // Deepsleep about 15 minutes or the full "DEEPSLEEP_HOW_MANY_HOURS"
        // "+- RF24_THIS_DEV_ADDRESS" = nodes doesnt wake up at the exact same time
        if (only_15_minutes) {
            deepsleep_target = 90 + RF24_THIS_DEV_ADDRESS;         
        } else {        
            deepsleep_target = (DEEPSLEEP_HOW_MANY_HOURS * 365) + RF24_THIS_DEV_ADDRESS;   
        }

        // Turn off everything
        WDT_RESET();
        radio_24.stopListening();
        radio_24.powerDown();
        delay(5);
        ADCSRA &= ~(1 << ADEN); // ADC 
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();

        // Wakeup by Watchdog every 8s (No other way while using internal clocks)
        // Go back to sleep if deepsleep_counter < deepsleep_target. Each cycle = 9.8s 
        while (deepsleep_counter < deepsleep_target) { 
            noInterrupts();
            sleep_bod_disable();

            // Clear various "reset" flags
            MCUSR = 0; 	
            WDTCSR = bit (WDCE) | bit(WDE);                 // WDT mode "interrupt" instead of "reset" 
            WDTCSR = bit (WDIE) | bit(WDP3) | bit(WDP0);    // WDT time interval (max is 8s)
            WDT_RESET(); 
            interrupts();
            sleep_cpu();  
        }
        // Morning!
        sleep_disable();                
        deepsleep_counter = 0;      
        delay(500);
        hardware.reset_devices();   // Reset device and all clocks
    }
#endif



namespace helper {

// Used <TimeLib.h> before, but ran out of progmem :/
void calc_time_until_deepsleep(uint16_t rf24_package_time) {
    WDT_RESET();
     
    // Time from master, formated "hhmm" to fit inside 2 bytes   
    uint16_t current_time = rf24_package_time;

    // Convert incomming hh:mm to hours and minutes left until DEEPSLEEP_AT_THIS_TIME
    int16_t hour_left = (DEEPSLEEP_AT_THIS_TIME / 100) - (current_time / 100);  
    int16_t minute_left = (DEEPSLEEP_AT_THIS_TIME % 100) - (current_time % 100);  

    if (minute_left < 0) {      // Negativ minute?
        hour_left--;            // - 1 from hour_left
        minute_left += 60;      // + 60 to minute_left
    }
    if (hour_left < 0) {        // Negativ hour?
        hour_left += 24;        // Add 24
    }
    // Deepsleep now?
    if (current_time > DEEPSLEEP_AT_THIS_TIME) {
        DEEPSLEEP();

    // Sleep at what millis()?   
    } else {
        uint32_t millis_left = (hour_left * 60UL + minute_left) * 60UL * 1000UL;    // Convert to milliseconds
        hardware.sleep_at_this_timestamp = millis_left + millis();                  // Add current millis() for absolute timestamp
        println("Time set!");            
    }  
}


// Deepsleep Watchdog interrupt service routine
#if ATTINY84_ENABLED  
    ISR (WDT_vect) {
        WDT_DISABLE();
        deepsleep_counter++;
    }
#endif

} // namespace helper



// ####################  DEBUG / CALIBRATION  ####################

// Battery ADC calibration
#if SERIAL_ENABLED && ADC_CAL_ENABLED
    void adc_calibration() { 
        WDT_DISABLE();
        digitalWrite(ADC_ENABLE_PIN, HIGH);

        while (1) {
            uint32_t sum = 0;
            for (uint8_t i = 0; i < 100; i++) {
                uint16_t value = analogRead(ADC_MEASURE_PIN); 
                sum += value;
                delay(10); 
            }
            uint16_t average = sum / 100;           
            print("Avg: "); println(average);
        }
    }


// Pump calibration
#elif SERIAL_ENABLED && PUMP_CAL_ENABLED
    void pump_calibration() {
        WDT_DISABLE(); 
        digitalWrite(PUMP_PIN, HIGH);
        ADCSRA |= (1 << ADEN); // Enable ADC
        
        // Loop forever
        while (1) {
            uint32_t sum = 0;
            uint32_t sum_raw = 0;
            for (uint8_t i = 0; i < 100; i++) {
                uint16_t raw_value = water_sensor.measure();
                uint16_t value = constrain(raw_value, WATER_SENSOR_MIN_VALUE, WATER_SENSOR_MAX_VALUE);
                sum += value;
                sum_raw += raw_value;
            }
            uint32_t water_level = sum / 100;
            uint32_t water_level_raw = sum_raw / 100; 
            
            bool raw = 1;
            
            // Raw 
            if (raw) {
                print("Raw sum: ");
                println(water_level_raw);

            // Mapped 
            } else {            
                print("Avg: "); 
                print(water_level); 
                print("\t\t\tMapped: ");
                uint32_t percent = map(water_level, WATER_SENSOR_MIN_VALUE, WATER_SENSOR_MAX_VALUE, 0, 100); //uint8_t
                print(percent);
                println("%");
            }
            delay(500); 
        }
    }
 

// Print message package
#elif SERIAL_ENABLED && !ATTINY84_ENABLED 
    void print_package(uint16_t* rf24_package_ptr) {
        println();
        println("#########  Package  #########");
        print("From who:      "); println(rf24_package_ptr[0]);
        print("to who:    "); println(rf24_package_ptr[1]);
        print("Int:         "); println(rf24_package_ptr[2]);
        print("Float:       "); println(rf24_package_ptr[3]);
        print("Bool:        "); println(rf24_package_ptr[4]);
        print("Time:        "); println(rf24_package_ptr[5]);
        println();
    }
#endif


