#include <Arduino.h>

//calibrate thresholds for each bot, PLEASE change the correct one 
//white and grey are the 2 bots, the colour of the mirror print
//comment out whichever one u r not testing 
//run this file
//the first number is for the temt in front, the next is the one to the right
//(?? possibly? if it looks weird then its the left one)
//if the temt is on a white line, the value will be quite high like >3000
//put on the green part and it will be lower
//so u set the threshold so that if reading > threshold, then it is white 
//set it abit lower (like 100 lower) so that the range for white is not too narrow


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

    pinMode(18, OUTPUT);
    digitalWrite(18, LOW);
}

int temt_pin[7] = {27, 34, 32, 13, 39, 14, 33}; //clockwise, start from front
// //white
// int thresholds[7] = {3000, 2400, 3000, 3000, 2900, 2900, 2900};

// //grey
int thresholds[7] = {3500, 3500, 3500, 3500, 3500, 3500, 3500};

void loop() {
    //a byte is 8 bits (0 or 1, aka binary)
    //data is basically 1 byte, if temt see line then 1, if no line then 0 
    //then u send over and on the pico side u covert back into 1 and 0 

    
    int number, ballcap;
    data = 0;
    for (int i = 0; i < 7; i++){
        if (analogRead(temt_pin[i]) >= thresholds[i]) number = 1; //white value
        else number = 0; 

        data += (number << i); 
    }

    if (analogRead(4) >= 20) ballcap = 1;
        else ballcap = 0; 

    data += (ballcap << 7);
    
    // Serial2.write(data);

    // delay(10);

  for (int i = 0; i < 7; i++){
    // if (analogRead(temt_pin[i]) > thresholds[i]) {
    //   Serial.print("1 ");
    //   digitalWrite(2, HIGH);
    // }
    // else {Serial.print("0 ");
      
      digitalWrite(2, LOW);
    Serial.print(analogRead(temt_pin[i]));
    Serial.print(" ");
    }
  Serial.println("");
  // Serial.println(data);

  if (Serial2.available()){
      if (Serial2.read() == 1){
        digitalWrite(18, HIGH);
        delay(50);
        digitalWrite(18, LOW);
      }

}
}
