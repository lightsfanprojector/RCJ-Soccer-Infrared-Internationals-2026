#include <Arduino.h>


uint8_t data = 0; 
// int white_value = 0; 
// int bcz_threshold = 0;

void setup() {
  // put your setup code here, to run once:
    Serial.begin(9600);
    Serial2.begin(9600, SERIAL_8N1, 16, 17);
    pinMode(27, INPUT);
    pinMode(34, INPUT);
    pinMode(39, INPUT);
    pinMode(33, INPUT);
    pinMode(32, INPUT);
    pinMode(13, INPUT);
    pinMode(25, INPUT);
    pinMode(4, INPUT);
}

int temt_pin[7] = {27, 34, 39, 33, 32, 13, 25}; //clockwise, start from front
//white
// int thresholds[7] = {3500, 1700, 3500, 3500, 3500, 3500, 3500};

// //grey
int thresholds[7] = {3500, 3000, 3500, 3000, 3500, 2200, 3500};

void loop() {
    Serial.println(analogRead(25));
    Serial.println("test");
}
