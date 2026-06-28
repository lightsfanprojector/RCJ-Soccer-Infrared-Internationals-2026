#include <Arduino.h>


void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);

    pinMode(18, OUTPUT);
    digitalWrite(18, LOW);
}



void loop() {
    Serial.println("test");
    digitalWrite(18, HIGH);
    delay(50);
    digitalWrite(18, LOW);
    delay(3000);

    
     
}
