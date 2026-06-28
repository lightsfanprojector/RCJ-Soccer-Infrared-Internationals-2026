#include <config.h>
#include <rcj_vision26.h>
#include <Arduino.h>

long long start_aim;

bool bc_state = false;

void setup() {
  Serial.begin(9600);
  Serial1.setTX(12);
  Serial1.setRX(13);
  Serial1.begin(115200);

//   // Serial.begin(115200, SERIAL_8N1, 13, 12); //to camera
//   Note: In the Arduino IDE, the USB serial port is referenced simply as Serial, 
//   while hardware UART ports are Serial1 and Serial2

  // Serial2.setTX(4);
  //   // int8_t rx = 5;
  //   // int8_t tx = 4;
  // Serial2.setRX(5);
  // Serial2.begin(9600);
  

  pinMode(22, INPUT);
  pinMode(34, INPUT);
  pinMode(1, OUTPUT); 
  
  pinMode(26, OUTPUT); 
  
  pinMode(27, OUTPUT); 
  
  pinMode(28, OUTPUT); 
  
  pinMode(18, OUTPUT); 
  pinMode(19, OUTPUT); 
  pinMode(20, OUTPUT); 
  pinMode(21, OUTPUT); 
  pinMode(25, OUTPUT);

  //two hardware serials for pico, serial1 (gpio1) for printing in monitor, serial (gpio0) for openmv (can set pins, openmv side uart)
  //ask if can use software serial (serial 2) for bluetooth comms with other bot????? or? 

  us_setup();
  i2c_setup();



}

void loop() {
  read_camera();
  // if (digitalRead(22) == 0) { //switch 
  //   drive2(0.0, 0.0, 0.0);
  // }
  // drive2(0.0, face_goal(read_camera()), 0.1);
  // drive(0.0, 0.1);
  // run_motor(0.1, 1, 26, 1);
  send_us();
  print_us();

}