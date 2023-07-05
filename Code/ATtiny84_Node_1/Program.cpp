


// Program.cpp
#include "Program.h"
#include "Config.h"


// Globals
unsigned long 
Sleep_at_this_millis = 0,           // Absolute timestamp 
ADC_at_this_millis = 0;             // Absolute timestamp
volatile int Deepsleep_count = 0;   // Counter while in deepsleep

// Objects
extern NRF24L01_radio RF24_radio;
extern Hardware_class Hardware;  
#if __ATTINY84_ON__ && __SERIAL_ON__
    SendOnlySoftwareSerial Tiny_serial(0);
#endif



//##################  NRF24L01  ##################

// Begin 
bool NRF24L01_radio::Begin_object() {
    Debug_println(__func__); 

    // Init
    if (!begin() && !isChipConnected()) {
        Debug_println("RF24 Offline!"); 
        return false; 
    }
    // Config
    setPALevel(RF24_OUTPUT);                                // Transmitter strength   
    setChannel(RF24_CHANNEL);                               // Radio channel 
    setDataRate(RF24_DATARATE);                             // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
    openWritingPipe(address[RF24_MASTER_NODE_ADDRESS]);     // Send to master = 0
    openReadingPipe(RF24_NODE_TYPE, address[1]);            // What pipe (RF24_NODE_TYPE) and address to listen on
    startListening();
    println("RF24 OK!");
    #if __SERIAL_ON__
        printDetails();
    #endif
    return true;
}


// Send message 
bool NRF24L01_radio::Send_message(uint16_t* RF24_package_ptr) {
    Debug_println(__func__); 
    for (uint8_t i=0; i < 6; i++) {

        // Insert Msg ID in the upper most 4 bits
        uint16_t Payload = (RF24_THIS_DEV_ADDRESS << 12) | RF24_package_ptr[i];
        
        // Send message(s)
        if (!write(&Payload, sizeof(Payload))) {
            for (uint8_t j = 0; j < 3; j++) {           // Retry loop foreach [i]                 
                delay(10);
                if (write(&Payload, sizeof(Payload))) { // Exit retry loop if successful
                    break;                                      
                } else if (j == 2) {                    // Failed 
                    return false;
                }  
            }
        }
        delay(10);      
    }
    return true;
} 


// Try to send message 
bool NRF24L01_radio::Try_send_message(/*uint16_t arg_to_who, uint16_t arg_int, uint16_t arg_float, uint16_t arg_state, uint16_t arg_time */) {
    Debug_println(__func__); 
    stopListening();
    
    // Create message array 
    uint16_t RF24_package[6]; 
    
    // Attiny84s (this node) have 2 bytes for Tx and Rx buffers...
    // splitting up package into small chunks of data                                
    RF24_package[0] = RF24_MASTER_NODE_ADDRESS;                     // To who
    RF24_package[1] = RF24_THIS_DEV_ADDRESS;                        // From who
    RF24_package[2] = 0;                                            // 
    RF24_package[3] = Hardware.Battery_charge_remaining();          // Bat charge remaining (ADC reading 0-1023)
    RF24_package[4] = 0;                                            // 
    RF24_package[5] = helper::Read_waterlevel_from_EEPROM(true);    // Current waterlevel calculate to % (1-100)

    for (uint8_t i = 0; i < 4; i++) {
        if (Send_message(RF24_package)) {
            Debug_println("Message OK!");
            return true;
        }
        delay(200);
    }
    Debug_println("Message Failed!");
    return false;
}


// Grab available RF24 message
bool NRF24L01_radio::Get_available_message(uint16_t* RF24_package_ptr) {
    unsigned long Timer = millis() + 5*1000UL;
    uint8_t CRC_check = 0;
    uint8_t CRC_sum = 0;
    uint16_t Payload;

    // Grab 6 packages in rapid succession
    for (uint8_t i = 0; i < 6; i++) {   
        read(&Payload, sizeof(Payload));
        CRC_check = (Payload >> 12) & 0x0F;         // Save only Msg ID
        CRC_sum += CRC_check;
        RF24_package_ptr[i] = Payload & 0x0FFF;     // Save only Msg
        flush_rx();          

        // Wait for next Msg. Break if time is up
        if (i < 5) {                                        
            while (!available()) {
                if (millis() > Timer) {
                    Debug_println("Error! Times up");
                    return false;
                }
            }
        // Expected "Checksum" ? 
        } else if (CRC_sum != 15) {
            Debug_println("Error! CRC check");
            return false;
        }
    }
    return true;
}


// Wait for incomming RF24 message 
bool NRF24L01_radio::Wait_for_message(uint16_t how_long, uint16_t* RF24_package_ptr) {
    unsigned long Timer = (millis() + how_long);
    uint8_t Pipe;
    WDT_RESET();

    // Start listening
    while (true) {
        if (available(&Pipe) && Pipe == RF24_NODE_TYPE) {
            return Get_available_message(RF24_package_ptr);
        }
        // Break after how_long milliseconds 
        if (millis() > Timer) {
            return false;
        }
    }
}


// Send battery and waterlevel status, get back the current time
void NRF24L01_radio::Send_data_get_time() {
    Debug_println(__func__);
    WDT_RESET(); 
    bool Error_flag = false;
    
    // Ask master node for time 
    Try_send_message();
    startListening();
    uint16_t RF24_package[6];
    uint8_t Loop_counter = 0;

    // Wait for response
    do {
        if (Wait_for_message(6000, RF24_package)     
        && ((RF24_package[0] == RF24_THIS_DEV_ADDRESS) 
        &&  (RF24_package[1] == RF24_MASTER_NODE_ADDRESS))) {

            // Code from master "Resend message"
            if (RF24_package[5] == 3333) {
                Send_data_get_time();

            // Time from master
            } else {
                helper::Calc_time_until_sleep(RF24_package);   
            }
            Error_flag = false;      
            
        } else { 
            Error_flag = true; 
        }
        Loop_counter++; 
    } while (Error_flag && Loop_counter < 4); // Wait for response from master
    
    // Still no answer?
    if (Error_flag) {
        Hardware.Reset_devices(false);
    }
}



//##################  Hardware  ##################

// Setup 
void Hardware_class::Setup() {

    // Reset Watchdog on boot
    #if __ATTINY84_ON__
        MCUSR &= ~(1<<WDRF);
        WDT_DISABLE();
    #endif
    
    // Serial
    #if __SERIAL_ON__
        SERIAL_BEGIN(9600);
        while (!Serial) {}
        println();
        delay(2000); 
    #endif

    // Physical I/O
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(PUMP_WATERTANK_PIN, INPUT);
    pinMode(ADC_ENABLE_PIN, OUTPUT);
    pinMode(ADC_MEASURE_PIN, INPUT);
    digitalWrite(PUMP_PIN, LOW);
    digitalWrite(ADC_ENABLE_PIN, LOW);
    
    // Begin RF24 radio 
    RF24_radio.Begin_object();

    // Begin EEPROM memory. Write only once, ever
    EEPROM.begin();
    if (EEPROM.read(3) == 255) {
        EEPROM.update(3, 42); // random nr !255
        EEPROM.update(PUMP_LEVEL_EEPROM_ADDRESS, PUMP_TIMES_UNTIL_EMPTY);
    }

    // Allow things to settle down, begin Watchdog
    delay(1000);                                                            
    WDT_ENABLE(WDTO_8S); 
}


// Get ADC reading from battery circuit
uint16_t Hardware_class::Battery_charge_remaining() { 
    Debug_println(__func__); 
    WDT_RESET();
    RF24_radio.stopListening();
    RF24_radio.powerDown();
    delay(5);
    
    // Toggle transistor pair to let current flow
    digitalWrite(ADC_ENABLE_PIN, HIGH);

    // Grab battery adc readings 
    uint32_t Sum = 0;
    for (uint8_t i = 0; i < 100; i++) {
        Sum += analogRead(ADC_MEASURE_PIN);
        delay(10); 
    }

    // Average ADC value (0-1023), all calculations are done on master (Save progmem)
    digitalWrite(ADC_ENABLE_PIN, LOW);
    RF24_radio.powerUp();
    delay(10);
    #if __ATTINY84_ON__
        uint16_t Charge_remaining = Sum / 100; 
        return Charge_remaining;
    #else
        return 737;
    #endif
}


// Start waterpump for param seconds
void Hardware_class::Start_water_pump(uint8_t How_long) {
    Debug_println(__func__);
    RF24_radio.stopListening(); 
    WDT_DISABLE();

    // Grab variables
    bool Watertank_empty = Is_watertank_empty();
    uint8_t Pump_counter = helper::Read_waterlevel_from_EEPROM();
    
    // Start pump 
    if (Pump_counter > 0 && !Watertank_empty) {

        // To keep the pump from flooding my apartment...
        if (How_long >= PUMP_TIMES_MAX) {
            How_long = 2;
        }
        // Toggle PN2222A on PUMP_PIN
        digitalWrite(PUMP_PIN, HIGH);
        delay(How_long * 1000);
        digitalWrite(PUMP_PIN, LOW); 
        delay(5);
        EEPROM.update(PUMP_LEVEL_EEPROM_ADDRESS, (Pump_counter - 1));
        WDT_ENABLE(WDTO_8S);
    }
}


// Hardware reset
void Hardware_class::Reset_devices(bool now) { 
    Debug_println(__func__);
    WDT_DISABLE();
    #if __ATTINY84_ON__

        // Delay 15mins
        if (!now) {
            delay(15*60*1000UL);
        }
        // Wait for watchdog to reset board
        WDT_ENABLE(WDTO_15MS);
        delay(1000); 
    #endif
}


// Check watertank
bool Hardware_class::Is_watertank_empty() {
    bool Prev_Status = EEPROM.read(PUMP_TANK_EEPROM_ADDRESS); 
    bool Cur_Status;

    // Check watertank float switch
    if (digitalRead(PUMP_WATERTANK_PIN) == HIGH) {
        Cur_Status = true;  // Tank is empty
    } else {
        Cur_Status = false; // Tank is not empty
    }
    // Watertank refilled?
    if (Prev_Status && !Cur_Status) {
        EEPROM.update(PUMP_LEVEL_EEPROM_ADDRESS, PUMP_TIMES_UNTIL_EMPTY);
    }
    // Write bool Is_watertank_empty to EEPROM
    EEPROM.update(PUMP_TANK_EEPROM_ADDRESS, Cur_Status);

    return Cur_Status;
}


// ATtiny84 + NRF24 deepsleep (Total board cunsumption about 9 µA)
#if __ATTINY84_ON__
    void Hardware_class::Deepsleep() {
        Debug_println(__func__); 

        // Turn off everything
        WDT_RESET(); 
        RF24_radio.stopListening();
        RF24_radio.powerDown();
        delay(10);
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_enable();

        // Wakeup by Watchdog every 8s, go back to sleep if Deepsleep_count < DEEPSLEEP_HOW_LONG. Each cycle = 9.8s
        // "+- RF24_THIS_DEV_ADDRESS" nodes doesnt wake up at the exact same time
        while (Deepsleep_count < (DEEPSLEEP_HOW_LONG * 365 + RF24_THIS_DEV_ADDRESS)) { 
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
        // 12 hours passed
        sleep_disable();                
        Deepsleep_count = 0;            // Reset deepsleep counter
        delay(500);
        Hardware.Reset_devices(true);   // millis() function doesnt reset to "0" otherwise
    }
#endif



namespace helper {

// Read in values from EEPROM memory
uint8_t Read_waterlevel_from_EEPROM(bool calc_percent) {
    uint16_t Value = EEPROM.read(PUMP_LEVEL_EEPROM_ADDRESS);
    bool Is_watertank_empty = EEPROM.read(PUMP_TANK_EEPROM_ADDRESS);

    // Empty row or empty watertank?
    if (Value == 255 || Is_watertank_empty) {
        Value = 0;
    }
    // Calculate remaining waterlevel to % (1-100)
    if (calc_percent) {
        Value = (Value * 100) / PUMP_TIMES_UNTIL_EMPTY;
    } 
    return Value;
}


// Used to have <TimeLib.h> for this but ran out of memory :/
void Calc_time_until_sleep(uint16_t* RF24_package_ptr) {
    Debug_println(__func__);
    WDT_RESET();
     
    // Time from master, formated "hhmm" to fit inside 2B   
    uint16_t Current_time = RF24_package_ptr[5];

    // Convert incomming hh:mm to hours and minutes left until DEEPSLEEP_AT_THIS_TIME
    int16_t Hour_left = (DEEPSLEEP_AT_THIS_TIME / 100) - (Current_time / 100);  
    int16_t Minute_left = (DEEPSLEEP_AT_THIS_TIME % 100) - (Current_time % 100);  

    if (Minute_left < 0) {      // Negativ minute?
        Hour_left--;            // Subtract 1 from Hour_left
        Minute_left += 60;      // Add 60 to Minute_left
    }
    if (Hour_left < 0) {        // Negativ hour?
        Hour_left += 24;        // Add 24
    }

    // Deepsleep now?
    if (Current_time > DEEPSLEEP_AT_THIS_TIME) {
        DEEPSLEEP();

    // Sleep at what millis()?   
    } else {
        unsigned long Millis_left;
        Millis_left = (Hour_left * 60UL + Minute_left) * 60UL * 1000UL; // Convert to millis
        Sleep_at_this_millis = Millis_left + millis();                  // Add current millis() to get absolute timestamp
        print("Time set: "); Debug_println(RF24_package_ptr[5]);        // Print
    }  
}


// Deepsleep Watchdog interrupt service routine
#if __ATTINY84_ON__  
    ISR (WDT_vect) {
        WDT_DISABLE();
        Deepsleep_count++;
    }
#endif

} // namespace helper



//####################  DEBUG  ####################

// ADC/volt calibration
#if __SERIAL_ON__ && __ADC_CAL_ON__
    void ADC_CAL_FUNC() { 
        WDT_DISABLE();
        digitalWrite(ADC_ENABLE_PIN, HIGH);

        while (1) {
            uint32_t Sum = 0;
            for (uint8_t i = 0; i < 100; i++) {
                uint16_t Value = analogRead(ADC_MEASURE_PIN); 
                Sum += Value;
                delay(10); 
            }
            uint16_t ADC_average = Sum / 100;           
            print("Avg: "); println(ADC_average);
        }
    }


// Print message package
#elif __SERIAL_ON__ && !__ATTINY84_ON__ && !__ADC_CAL_ON__
    void Print_package(uint16_t* RF24_package) {
        println();
        println("#########  Package  #########");
        print("To who:      "); println(RF24_package[0]);
        print("From who:    "); println(RF24_package[1]);
        print("Int:         "); println(RF24_package[2]);
        print("Float:       "); println(RF24_package[3]);
        print("Bool:        "); println(RF24_package[4]);
        print("Time:        "); println(RF24_package[5]);
        println();
    }
#endif



