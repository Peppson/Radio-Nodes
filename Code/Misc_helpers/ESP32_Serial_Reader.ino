
// ######## ESP32 RX Pin = Gpio 14 ########
// Budget Serial reader using an ESP32

int8_t TX_Pin = 14;
int8_t RX_Pin = -1;
unsigned long Prev_millis;
unsigned long Timer;


// Setup
void setup() {
    Serial.begin(9600);                                 // USB Serial
    Serial1.begin(9600, SERIAL_8N1, TX_Pin, RX_Pin);    // Incomming Serial
    pinMode(2, OUTPUT);                                 // Onboard led
    delay(4000);
    for (uint8_t i=0; i<3; i++) {
        digitalWrite(2, HIGH);
        delay(300);
        digitalWrite(2, LOW);
        delay(300);
    }
    for (uint8_t i=0; i<25; i++) {
        Serial.println();
    }
    Serial.println("###########  READY!  ###########");
}


// Main loop
void loop() {
    // Read data from the ATtiny84's serial out
    if (Serial1.available()) {
        char data = Serial1.read(); 

        // Forward data to the USB serial connection + budget timer 
        if (data == '<') {
            Serial.println("### Timer started! ###");
            Prev_millis = millis();
        }
        else if (data == '>') {
            Timer = (millis() - Prev_millis);
            Serial.print("### Timer stopped! ###"); Serial.print("     "); Serial.print(Timer/1000); Serial.print("s");
            Serial.print("      "); Serial.print("ms = "); Serial.println(Timer); 
        }
        else {
            Serial.print(data); 
        }
    }
}


