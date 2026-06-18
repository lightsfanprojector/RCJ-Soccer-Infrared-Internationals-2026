// #include <Wire.h>
// #include <Arduino.h>

// void setup(){
//     Serial.begin(9600);
//     Wire1.setSCL(3);
//     Wire1.setSDA(2);
//     Wire1.begin();


// }

// void loop(){
    
//   Wire1.beginTransmission(0x3B);
//   Wire1.requestFrom(0X3B, 2);
//   Wire1.endTransmission();
//   delay(1);

//   while (Wire1.available() < 2){
//   }
//   Serial.println((Wire1.read() * 255) + Wire1.read());


// }
#include <Wire.h>
#include <Arduino.h>

void setup() {
    Serial.begin(9600);
    // Initialize Wire1 with your specific pins
    Wire1.setSCL(3);
    Wire1.setSDA(2);
    Wire1.begin();
    
    // Note: Most sensors (like MPU6050) need a "wake up" command 
    // to the power management register before they send data.
}

void loop() {
    // 1. Point to the register you want to read (0x3B is usually Accel X High)
    Wire1.beginTransmission(0x3B);
    Wire1.write(0x3B); 
    Wire1.endTransmission(false); // Send a restart, not a stop

    // 2. Request 2 bytes
    if (Wire1.requestFrom(0x3B, 2) == 2) {
        byte highByte = Wire1.read();
        byte lowByte = Wire1.read();

        // 3. Combine bytes correctly: (High << 8) | Low
        int16_t combinedValue = (highByte << 8) | lowByte;

        Serial.println(combinedValue);
    }

    delay(500); // Slow down for readability
}