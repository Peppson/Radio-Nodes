
/*
#############  Homebrew Budget 433Mhz_Sniffer (Decoder?)  #############
*/

#define RF433_TX_PIN 26
#define NUM 2500UL
#define THRESHOLD 200

uint16_t array[132];
unsigned long old;
unsigned long new;
uint16_t sample;
void transmit_RF_remote_codes(uint8_t retries = 3);


// Capture 433Mhz 
#pragma GCC optimize ("unroll-loops")
bool capture() {
    old = micros(); 

    // Where in Msg? end = 9300 micros
    while (micros() - old < 9100UL) {
        if (analogRead(A0) > NUM) {
            old = 0;
            break;
        }
    }
    // Start capture 
    if (micros() - old >= 9000UL) {
        bool error_flag = false;
        for (uint8_t i = 0; i < 132; i += 2) {

            while (analogRead(A0) < NUM) {}
            old = micros();

            while (analogRead(A0) > NUM) {}
            sample = micros() - old;

            new = micros();
            array[i] = sample;

            while (analogRead(A0) < NUM) {}
            sample = micros() - new;
            array[i + 1] = sample;
        }    
        
        // Clear console
        for (uint8_t i = 0; i < 25; i++) {
            Serial.println();
        }
        
        // Quantize captured data
        Serial.print("Actual:       "); 
        for (uint8_t i = 0; i < 131; i++) {
            uint16_t array = array[i];

            if (array < 240) {
                array[i] = 230;
            } else if (array > 250 && array <= 600) {
                array[i] = 275;
            } else if (array > 600 && array <= 2000) {
                array[i] = 1325;
            } else if (array > 2000 && array <= 3500) {
                array[i] = 2575;
            }
            else if (i < 8) {
                error_flag = true;
                break;
            }
            // Print actual capture
            Serial.print(array); Serial.print(", ");                
        }

        // Print.exe
        if (!error_flag) {
            Serial.println();
            Serial.print("Quantized:    ");
            
            // Print quantized capture 
            for (int i =0 ; i < 132; i++) {
                Serial.print(array[i]); Serial.print(", ");     
            }
            Serial.println();
            Serial.println();
            delay(5000);
            Serial.println();
            Serial.println("Trying Captured code!!");
            transmit_RF_remote_codes();
        } else {
            Serial.println();
            Serial.println("Error!");
            delay(3000);
            return false;
        }
    }
    return true;
}


// Transmit and test if valid 
#pragma GCC optimize ("unroll-loops")
void transmit_RF_remote_codes(uint8_t retries) {

    // Number of retries
    for (uint i = 0; i < retries; i ++) {

        // Loop through the array
        for (uint8_t j = 0; j < 131; j += 2) {            
            uint16_t high_duration = array[j];
            uint16_t low_duration = array[j + 1];

            // High pulse
            digitalWrite(RF433_TX_PIN, HIGH);
            delayMicroseconds(high_duration);

            // Low pulse
            digitalWrite(RF433_TX_PIN, LOW);
            delayMicroseconds(low_duration);
        }
        // Paus between retries, simulates "End pulse"
        delayMicroseconds(9300); 
    }
}


// Try to match incomming signal with known "buttons"
void what_button() {
    uint8_t compare_score = 0;
    uint8_t what_button = 42;

    // Compare to each known array
    for (uint8_t i = 0; i < 6; i++) {
        compare_score = 0;
        
        // Only compare the last 25 indexes
        for (uint8_t j = (131-24); j < 131; j++) {
            if (abs(Secret_RF_remote_codes[i][j] - array[j]) <= THRESHOLD) {
                compare_score += 1; 
            }   
        }
        // 22/24 or more is ok
        if (compare_score > 22) {
            what_button = i;
            break;
        }
    }
    // Print.exe
    Serial.println();
    switch (what_button) {
        case 0:
            Serial.println("Button 1: On");
            break;
        case 1:
            Serial.println("Button 1: OFF");
            break;
        case 2:
            Serial.println("Button 2: On");
            break;
        case 3:
            Serial.println("Button 2: OFF");
            break;
        case 4:
            Serial.println("Button 3: On");
            break;
        case 5:
            Serial.println("Button 3: OFF");
            break;
        case 6:
            Serial.println("Button ALL OFF");
            break;
        default:
            Serial.println("Error!");
            break;   
    }
    // Clear array
    for (uint8_t i = 0; i < 131; i++) {
        array[i] = 0; 
    }
}


// Setup
void setup() {
  Serial.begin(9600);
  delay(1000);
  pinMode(RF433_TX_PIN, OUTPUT);
    for (uint8_t i=0; i<25; i++) {
        Serial.println();
    }
  Serial.println("Starting");
}


// Main loop
void loop() {
    // Listen for Msg
    if (analogRead(A0) > NUM + 500) {
        if (capture()) {
            what_button(); // Try to match incomming signal with known "buttons"
        }
        Serial.println("Ready");  
    }
}


