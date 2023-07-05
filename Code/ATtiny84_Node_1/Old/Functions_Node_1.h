
/*
########################  Node 1 Functions  #########################
#                                                                   #
#   - Flower_waterpump, controlled via Home Assistant               #                                          
#   - Library, TMRh20/RF24: https://github.com/tmrh20/RF24/         #     
#   - Class config: https://nrf24.github.io/RF24/classRF24.html     #     
#   - NRF24L01 Address "1"                                          #
#                                                                   #
#####################################################################
*/

// Functions_Node_1
#pragma once
#include "Arduino.h"


// Setup 
void Setup_everything() {

    // Reset Watchdog on boot
    #if ATtiny84_ON
        MCUSR &= ~(1<<WDRF);
        wdt_disable();
    #endif

    // Pin setup
    pinMode(PUMP_PIN, OUTPUT);
    pinMode(ADC_Enable_Pin, OUTPUT);
    pinMode(ADC_Measure_PIN, INPUT);
    digitalWrite(ADC_Enable_Pin, LOW);
    digitalWrite(PUMP_PIN, LOW);

    // Serial
    #if SERIAL_ON
        SERIAL_BEGIN(9600);
        while (!Serial) {} 

        // Fill console with /n
        for(uint8_t i=0; i<20; i++) {
            println();
        }
        // Radio
        if (!radio.begin() || !radio.isChipConnected()) {
            println("Where radio? :(");
            while (true) {}    
        }
        println("Radio OK!"); 
    #else
        // Serial off 
        if (!radio.begin() || !radio.isChipConnected()) {
            while (true) {}  
        }  
    #endif

    // Radio setup
    radio.setPALevel(Radio_output);                                         // Transmitter strength   
    radio.setChannel(Radio_channel);                                        // Radio channel (above wifi) 
    radio.setDataRate(RF24_2MBPS);                                          // Datarate: RF24_2MBPS, RF24_1MBPS, RF24_250KBPS
    radio.openWritingPipe(address[Master_node_address]);                    // Send to master = 0
    radio.openReadingPipe(This_dev_address, address[This_dev_address]);     // What pipe to listen on
    delay(4000);                                                            // Allow time for things to settle down
    
    // Watchdog
    #if ATtiny84_ON
        wdt_enable(WDTO_8S); 
    #endif
    println("Started!");  
}


// Message received, start waterpump for n seconds
void Start_water_pump(uint8_t How_long = 2){
    println("Pump");
    WDT_RESET();
    radio.stopListening();

    // To keep the pump from flooding my apartment...
    if (How_long >= Pump_runtime_max) {
        How_long = 2;
    }
    // Toggle transistor on PUMP_PIN
    wdt_disable(); 
    digitalWrite(PUMP_PIN, HIGH);
    delay(How_long * 1000);
    digitalWrite(PUMP_PIN, LOW);
    wdt_enable(WDTO_8S); 
    delay(5);  
}


// Get ADC reading from battery
uint16_t Battery_charge_remaining() {
    println("Bat"); 
    WDT_RESET();
    radio.stopListening();
    radio.powerDown();
    delay(10);
    
    // Toggle on ADC transistor to let current flow to voltage divider
    digitalWrite(ADC_Enable_Pin, HIGH);

    // Grab battery reading while radio is powered off
    uint32_t Sum = 0;
    for (uint8_t i = 0; i < 100; i++) {
        uint16_t Value = analogRead(ADC_Measure_PIN); 
        Sum += Value;
        delay(10); 
    }
    // Average ADC value (0-1023), all calculations are done on master to save mem
    digitalWrite(ADC_Enable_Pin, LOW);
    uint16_t Charge_remaining = Sum / 100;  
    radio.powerUp();
    delay(10);
    return Charge_remaining;
}
    

// Send message 
bool Send_message(uint16_t Arg_float) {
    println("Send");

    // Insert variables     
    Message_package[0] = Master_node_address;       // To who
    Message_package[1] = This_dev_address;          // From who
    Message_package[2] = 0;                         // Node_1 run pump for how long     (Only on RX)
    Message_package[3] = Arg_float;                 // Battery charge remaining         (ADC reading 0-1023)
    Message_package[4] = 0;                         // Bool on/off                      (Only on RX)
    Message_package[5] = 0;                         // Current time                     (Only on RX)
      
    // Send message(s)
    for (uint8_t i=0; i < 6; i++) {
        // Insert Msg ID in the upper most 4 bytes
        uint16_t Content = (This_dev_address << 12) | Message_package[i];
        if (!radio.write(&Content, sizeof(Content))) {
            // Retry loop foreach [i]
            for (uint8_t j=0; j < 3; j++) {                     
                delay(25);
                // Exit retry loop if successful
                if(radio.write(&Content, sizeof(Content))) {
                    break;                                      
                }
                else if (j == 2) {
                    return false;
                }  
            }
        }
        delay(1);      
    }
    return true;
} 


// Try to send message n times
bool Try_send_message() {
    println("Try send");
    uint16_t Charge_remaining = Battery_charge_remaining();
    for (uint8_t i = 0; i < 4; i++) {
        if (Send_message(Charge_remaining)) {
            println("Msg successful!!");
            return true;
        }
        delay(200);
    }
    println("Msg failed!!");
    return false;
}


// Hardware reset
void Hardware_reset_after_15min(uint16_t Seconds = 60 * 15) { 
    println("Hardware reset");
    #if ATtiny84_ON
        unsigned long Timeout = 1000UL * Seconds; 
        wdt_disable();
        delay(Timeout);
        wdt_enable(WDTO_15MS);
        delay(1000); // Wait for watchdog to reset board
    #endif
}


// Watchdog ISR 
#if ATtiny84_ON
ISR (WDT_vect) {
    wdt_disable();
    Deepsleep_count ++;
}


// ATtiny84 + NRF24 deepsleep (about 5 µA)
void Deepsleep() {
    println("Deepsleep");

    // Turn off everything
    WDT_RESET(); 
    radio.stopListening();
    radio.powerDown();
    delay(10);
    static byte prevADCSRA = ADCSRA;
    ADCSRA = 0;
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);
    sleep_enable();

    // Wake by watchdog every 8s, go back to sleep if Deepsleep_count < Sleep_how_long. Each cycle = 9.8s
    while (Deepsleep_count < (Sleep_how_long * 367)) { 
        noInterrupts();
		sleep_bod_disable();

		// Clear various "reset" flags
		MCUSR = 0; 	
		WDTCSR = bit (WDCE) | bit(WDE);                 // WDT mode "interrupt" instead of "reset" 
		WDTCSR = bit (WDIE) | bit(WDP3) | bit(WDP0);    // WDT time interval (max 8s)
        WDT_RESET(); 
		interrupts();
		sleep_cpu();  
	}
    // 12 hours passed
	sleep_disable();                // Sleep disable
	Deepsleep_count = 0;            // Reset deepsleep counter
	ADCSRA = prevADCSRA;            // Re-enable ADC
    delay(500);
    Hardware_reset_after_15min(1);  // millis() doesnt reset otherwise
}
#endif


// Get available message
bool Get_available_message() {
    unsigned long Msg_timer = millis() + 5 * 1000UL;
    uint16_t CRC_check;
    uint16_t CRC_sum = 0;
    uint16_t Message;
    
    // Grab 6 packages in rapid succession
    for (uint8_t i=0; i<6; i++) {
        if (radio.available()) {     
            radio.read(&Message, sizeof(Message));
            Message_package[i] = Message & 0x0FFF;  // Save only Msg
            CRC_check = (Message >> 12) & 0x0F;     // Save only Msg ID
            CRC_sum += CRC_check;              
        }
        // Wait for next Msg. Break if time is up
        if (i < 5) {                                        
            while (!radio.available()) {
                if (millis() > Msg_timer) {
                    return false;
                }
            }  
        }
        // Expected "Checksum" ?  
        else if (CRC_sum != 15) {
            print("CRC Error :"); println(CRC_sum);
            return false;
        }
    }
    return true;
}


// Wait for message 
bool Wait_for_message(uint16_t Offset) {
    unsigned long Msg_wait_timer = (millis() + Offset);
    uint8_t Pipe;

    // Start listening
    while (true) {
        if (radio.available(&Pipe) && Pipe == This_dev_address) {
            return Get_available_message();
        }
        // Break after Offset ms
        if (millis() > Msg_wait_timer) {
            return false;
        }
    }
}


// Used to have <TimeLib.h> for this but ran out of memory :/
void Calc_time_until_sleep() {
    println("Calc sleep");

    // Time from master (uint16_t) formated "hhmm"     
    int16_t Current_time = Message_package[5];

    // Convert incomming msg to: hours and minutes left until Sleep_time
    int16_t Hour_left = (Sleep_time / 100) - (Current_time / 100);  
    int16_t Minute_left = (Sleep_time % 100) - (Current_time % 100);  

    if (Minute_left < 0) {      // Negativ minute?
        Hour_left--;            // Subtract 1 from Hour_left
        Minute_left += 60;      // Add 60 to Minute_left
    }
    if (Hour_left < 0) {        // Negativ hour?
        Hour_left += 24;        // Add 24
    }

    // Is Current_time greater than Sleep_time?
    if (Current_time > Sleep_time) {
        Hour_left = 0;
        Minute_left = 0;   
    }
    // Sleep at what time?
    unsigned long Millis_left = (Hour_left * 60UL + Minute_left) * 60UL * 1000UL; // Convert to millis
    Sleep_at_this_millis = Millis_left + millis(); // Add current millis() to get an absolute timestamp
    
}


// Send battery charge remaining, get back current time
void Send_ADC_get_time() {
    println(); println("Get time");
    WDT_RESET();

    // Get time from master or hard reset
    for (uint8_t i=0; i<2; i++){ 
        if (Try_send_message()) {
            break;
        }
        else if (i == 1){
            Hardware_reset_after_15min();
        }
        delay(6000);
    }
    // Message successful! Listening for response
    radio.flush_rx();
    radio.flush_tx();
    radio.startListening();
    delay(5); 

    // Wait for return message 
    if (Wait_for_message(3000)) {
        println("Return Msg OK!");
        if ((Message_package[0] == This_dev_address) && (Message_package[1] == Master_node_address)) {
            Calc_time_until_sleep(); // Calculate time until deepsleep, dependent on the current time
            println("Time set!");
            radio.flush_rx();
            radio.flush_tx();
            delay(5); 
        } 
    } 
    else {
        Hardware_reset_after_15min(); 
    }
}


// ADC CAL DEBUG FUNC
#if ADC_CAL_ON
    void ADC_CAL_FUNC() {
        wdt_disable();
        digitalWrite(ADC_Enable_Pin, HIGH);

        while (1) {
            uint32_t Sum = 0;
            for (uint8_t i = 0; i < 100; i++) {
                uint16_t Value = analogRead(ADC_Measure_PIN); 
                Sum += Value;
                delay(10); 
            }
            uint16_t ADC_average = Sum / 100;           
            print("Avg: "); println(ADC_average);
        }
    }
#endif

