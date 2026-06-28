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

  //comment out whatever u r not testing in here, ask for help if u not sure
  //whichever bot u r testing, u need to go to config, and uncomment the section for that bot 

  read_camera(); //check if we r getting goal angles from camera
  //if this is not working then its probably the camera thresholds, download openmv IDE and put 'untitled_1.py' into it, 
  //then change the colour thresholds for yellow and blue using the actual goal on the field (in threshold editor im sure u guys rmb this)
  //check that it can see the colour even if its far away 

  send_us();
  print_us(); //check that all US are correct, put bot on the field and check if coordinates are correct

  //check ball orbit: 
  drive(orbit_ball(), 0.2);

  //check compass angle is correct:
  Serial.println(compass_angle());

  //calibrate FRONT_LIDAR_MAX & BACK_LIDAR_MAX: run each one separately, for each bot also, change in config
  //pls change correct one, for WHITE and GREY bot 
  Serial.println(getirfront(9)); //front
  Serial.println(getirback(9)); //back
  
  //once u r done run the actual code (striker.cpp and goalie.cpp)
  //check acceleration and kicker stuff 
  //if ball in ballcap, it will turn to face opponents goal, accelerate, & kick if its close enough to the goal







  //ignore
  // if (digitalRead(22) == 0) { //switch 
  //   drive2(0.0, 0.0, 0.0);
  // }
  // drive2(0.0, face_goal(read_camera()), 0.1);
  // drive(0.0, 0.1);
  // run_motor(0.1, 1, 26, 1);

  



}