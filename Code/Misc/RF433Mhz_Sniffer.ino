
/*
#############  Homebrew Budget 433Mhz_Sniffer (Decoder?)  #############
*/

#define RF433_TX_PIN 26
#define NUM 2500UL
#define Threshold 200

uint16_t Array[132];
unsigned long Old;
unsigned long New;
uint16_t Sample;
void Transmit_RF_remote_codes(uint8_t retries = 3);


// 2D array with captured 433Mhz remote codes, alot of averages
const uint16_t Secret_RF_remote_codes[7][132] = {
    
    // Button 0 ON
        {232, 2578, 236, 274, 237, 1291, 235, 1282, 235, 283, 236, 1283, 236, 281, 239, 1281, 233, 284, 236, 274, 237, 1292, 236, 1284, 234, 282, 238, 1281, 234, 284, 237, 272, 238, 1302, 236, 274, 235, 1293, 235, 274, 235, 1293, 235, 274, 235, 1293, 236, 1282, 233, 284, 236, 1283, 235, 284, 235, 1283, 233, 284, 235, 1285, 235, 283, 234, 276, 234, 1304, 236, 1283, 234, 284, 234, 1285, 232, 285, 235, 1284, 233, 284, 236, 275, 234, 1295, 234, 274, 235, 1292, 233, 277, 235, 1293, 233, 276, 234, 1293, 234, 1285, 232, 297, 234, 1285, 234, 284, 235, 275, 234, 1295, 230, 278, 233, 1294, 232, 1286, 233, 285, 234, 275, 235, 1292, 234, 276, 233, 1295, 232, 278, 232, 1295, 231, 278, 233, 1286, 230, 9270}, 
    // Button 0 OFF
        {238, 2572, 241, 267, 243, 1285, 241, 1278, 242, 276, 241, 1278, 241, 276, 245, 1275, 242, 276, 243, 267, 242, 1287, 240, 1278, 240, 278, 241, 1278, 241, 278, 239, 270, 241, 1299, 240, 268, 240, 1290, 239, 270, 240, 1288, 239, 271, 239, 1289, 238, 1280, 240, 280, 238, 1279, 238, 281, 242, 1277, 240, 279, 240, 1279, 239, 278, 242, 270, 239, 1300, 238, 1280, 239, 280, 239, 1280, 238, 280, 240, 1280, 237, 281, 238, 271, 237, 1291, 237, 272, 238, 1288, 238, 273, 237, 1291, 239, 271, 238, 1290, 236, 1282, 237, 292, 237, 1283, 238, 280, 239, 271, 239, 1291, 236, 273, 239, 1289, 234, 274, 238, 1292, 237, 272, 239, 1289, 236, 274, 236, 1291, 238, 273, 237, 1290, 237, 273, 237, 1283, 233, 9271},
    // Button 1 ON
        {239, 2573, 240, 269, 241, 1288, 240, 1279, 239, 278, 242, 1277, 239, 280, 240, 1278, 239, 279, 240, 271, 240, 1289, 239, 1280, 239, 278, 239, 1279, 239, 281, 239, 270, 239, 1301, 240, 270, 239, 1288, 239, 271, 240, 1288, 238, 271, 238, 1288, 239, 1281, 238, 281, 238, 1280, 237, 281, 239, 1281, 238, 279, 240, 1280, 238, 281, 239, 270, 239, 1300, 237, 1281, 239, 279, 239, 1282, 237, 281, 237, 1282, 238, 280, 238, 272, 239, 1289, 239, 272, 238, 1289, 239, 271, 237, 1292, 237, 273, 239, 1288, 236, 1282, 237, 293, 237, 1281, 237, 282, 238, 272, 237, 1291, 235, 274, 237, 1290, 236, 1283, 237, 282, 238, 271, 237, 1291, 236, 273, 234, 1292, 235, 276, 236, 1292, 235, 1284, 237, 273, 232, 9271},
    // Button 1 OFF
        {237, 2575, 238, 269, 239, 1289, 239, 1280, 238, 280, 240, 1279, 239, 280, 237, 1282, 239, 280, 240, 268, 241, 1288, 240, 1278, 238, 280, 240, 1280, 236, 282, 239, 272, 238, 1300, 237, 272, 239, 1288, 238, 272, 238, 1289, 239, 271, 237, 1291, 239, 1279, 235, 283, 237, 1281, 238, 279, 238, 1281, 237, 282, 236, 1283, 238, 280, 239, 270, 238, 1301, 237, 1280, 237, 282, 238, 1282, 236, 283, 237, 1281, 236, 282, 237, 273, 239, 1290, 236, 272, 237, 1290, 236, 274, 235, 1291, 237, 272, 238, 1290, 235, 1283, 235, 294, 235, 1283, 236, 282, 237, 272, 236, 1292, 235, 273, 235, 1293, 235, 275, 235, 1293, 235, 273, 236, 1292, 235, 274, 236, 1292, 233, 277, 233, 1292, 235, 1285, 233, 275, 232, 9270},
    // Button 2 ON
        {240, 2571, 238, 269, 241, 1287, 243, 1274, 242, 277, 240, 1277, 242, 277, 241, 1279, 240, 277, 241, 269, 242, 1286, 241, 1277, 241, 277, 242, 1276, 241, 277, 243, 266, 243, 1296, 240, 269, 242, 1286, 239, 270, 240, 1289, 238, 270, 240, 1287, 241, 1277, 240, 279, 241, 1276, 239, 280, 239, 1280, 239, 278, 240, 1280, 237, 279, 240, 271, 241, 1297, 238, 1280, 239, 279, 239, 1280, 238, 279, 240, 1281, 238, 280, 238, 271, 238, 1290, 239, 271, 238, 1289, 239, 271, 240, 1289, 237, 270, 240, 1289, 236, 1281, 240, 290, 239, 1279, 235, 282, 239, 272, 239, 1288, 236, 273, 237, 1290, 238, 1279, 238, 281, 239, 270, 238, 1290, 236, 274, 236, 1291, 237, 1281, 236, 283, 237, 272, 234, 1284, 235, 9266},        
    // Button 2 OFF
        {234, 2575, 238, 272, 236, 1292, 237, 1280, 239, 280, 239, 1278, 239, 280, 239, 1280, 238, 280, 239, 270, 239, 1289, 237, 1283, 237, 280, 239, 1282, 238, 280, 238, 270, 238, 1302, 239, 271, 237, 1292, 237, 272, 238, 1289, 237, 274, 238, 1289, 237, 1283, 235, 282, 237, 1282, 237, 282, 237, 1282, 237, 280, 238, 1282, 234, 283, 236, 274, 236, 1304, 234, 1283, 235, 284, 238, 1281, 236, 283, 237, 1280, 236, 282, 237, 275, 235, 1292, 236, 275, 235, 1292, 234, 275, 237, 1290, 235, 274, 237, 1291, 236, 1283, 235, 295, 235, 1285, 234, 284, 235, 275, 234, 1295, 233, 275, 236, 1293, 232, 275, 237, 1291, 235, 274, 234, 1294, 234, 276, 233, 1293, 235, 1284, 233, 285, 233, 276, 235, 1284, 231, 9277},
    // Button ALL OFF
        {230, 2584, 232, 277, 232, 1297, 232, 1288, 233, 285, 233, 1286, 234, 286, 234, 1288, 232, 286, 234, 276, 233, 1296, 233, 1288, 232, 286, 233, 1288, 232, 286, 233, 277, 234, 1307, 233, 276, 233, 1297, 231, 279, 233, 1296, 232, 279, 232, 1297, 232, 1288, 231, 288, 232, 1288, 232, 287, 231, 1290, 231, 287, 232, 1287, 232, 289, 231, 279, 231, 1310, 231, 1289, 230, 288, 231, 1290, 231, 288, 232, 1288, 231, 287, 230, 280, 232, 1298, 230, 280, 229, 1300, 230, 280, 230, 1299, 230, 279, 230, 1301, 229, 1290, 229, 300, 229, 1290, 230, 290, 230, 280, 231, 1300, 229, 1293, 229, 291, 230, 281, 228, 1301, 228, 281, 229, 1300, 231, 279, 229, 1300, 229, 282, 230, 1299, 229, 281, 229, 1289, 227, 9287},   
};


// Capture 433Mhz 
#pragma GCC optimize ("unroll-loops")
bool Capture() {
    Old = micros(); 

    // Where in Msg? end = 9300 micros
    while (micros() - Old < 9100UL) {
        if (analogRead(A0) > NUM) {
            Old = 0;
            break;
        }
    }
    // Start capture 
    if (micros() - Old >= 9000UL) {
        bool Error_flag = false;
        for (uint8_t i = 0; i < 132; i += 2) {

            while (analogRead(A0) < NUM) {}
            Old = micros();

            while (analogRead(A0) > NUM) {}
            Sample = micros() - Old;

            New = micros();
            Array[i] = Sample;

            while (analogRead(A0) < NUM) {}
            Sample = micros() - New;
            Array[i + 1] = Sample;
        }    
        
        // Clear console
        for (uint8_t i = 0; i < 25; i++) {
            Serial.println();
        }
        
        // Quantize captured data
        Serial.print("Actual:       "); 
        for (uint8_t i = 0; i < 131; i++) {
            uint16_t array = Array[i];

            if (array < 240) {
                Array[i] = 230;
            } else if (array > 250 && array <= 600) {
                Array[i] = 275;
            } else if (array > 600 && array <= 2000) {
                Array[i] = 1325;
            } else if (array > 2000 && array <= 3500) {
                Array[i] = 2575;
            }
            else if (i < 8) {
                Error_flag = true;
                break;
            }
            // Print actual capture
            Serial.print(array); Serial.print(", ");                
        }

        // Print.exe
        if (!Error_flag) {
            Serial.println();
            Serial.print("Quantized:    ");
            
            // Print quantized capture 
            for (int i =0 ; i < 132; i++) {
                Serial.print(Array[i]); Serial.print(", ");     
            }
            Serial.println();
            Serial.println();
            delay(5000);
            Serial.println();
            Serial.println("Trying Captured code!!");
            Transmit_RF_remote_codes();
        } else {
            Serial.println();
            Serial.println("Error!");
            delay(3000);
            return false;
        }
    }
    return true;
}


// Transmit
#pragma GCC optimize ("unroll-loops")
void Transmit_RF_remote_codes(uint8_t retries) {

    // Number of retries
    for (uint i = 0; i < retries; i ++) {

        // Loop through the array
        for (uint8_t j = 0; j < 131; j += 2) {            
            uint16_t High_duration = Array[j];
            uint16_t Low_duration = Array[j + 1];

            // High pulse
            digitalWrite(RF433_TX_PIN, HIGH);
            delayMicroseconds(High_duration);

            // Low pulse
            digitalWrite(RF433_TX_PIN, LOW);
            delayMicroseconds(Low_duration);
        }
        // Paus between retries, simulates "End pulse"
        delayMicroseconds(9300); 
    }
}


// Try to match incomming signal with known "buttons"
void What_button() {
    uint8_t Compare_score = 0;
    uint8_t What_button = 42;

    // Compare to each known array
    for (uint8_t i = 0; i < 6; i++) {
        Compare_score = 0;
        
        // Only compare the last 25 indexes
        for (uint8_t j = (131-24); j < 131; j++) {
            if (abs(Secret_RF_remote_codes[i][j] - Array[j]) <= Threshold) {
                Compare_score += 1; 
            }   
        }
        // 22/24 or more is ok
        if (Compare_score > 22) {
            What_button = i;
            break;
        }
    }
    // Print.exe
    Serial.println();
    switch (What_button) {
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
        Array[i] = 0; 
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
  Serial.println("Start");
}


// Main loop
void loop() {
    // Listen for Msg
    if (analogRead(A0) > NUM + 500) {
        if (Capture()) {
            What_button(); // Try to match incomming signal with known "buttons"
        }
        Serial.println("Ready");  
    }
}


