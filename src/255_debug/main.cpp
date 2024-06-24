
#include "../global_config.h"


void setup() {
    Serial.begin(9600, (SERIAL_8N1 | SERIAL_TX_ONLY));
    while (!Serial) { }
    delay(100);
    log("\n-- Node %i --\n", RF24_THIS_ADDRESS);
        
}

void loop() {

}
  
