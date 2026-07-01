#include <config.h>
#include <rcj_vision26.h>
#include <Arduino.h>

long long start_aim;

bool bc_state = false;
long long timeoflastkick = 0;

void setup() {

  // Serial1.begin(9600);

//   // Serial.begin(115200, SERIAL_8N1, 13, 12); //to camera
//   Note: In the Arduino IDE, the USB serial port is referenced simply as Serial, 
//   while hardware UART ports are Serial1 and Serial2
  Serial2.setTX(4);
    // int8_t rx = 5;
    // int8_t tx = 4;
  Serial2.setRX(5);
  Serial2.begin(9600);

  Serial1.setTX(12);
  Serial1.setRX(13);
  Serial1.begin(115200);

  Serial.begin(9600);

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
  pinMode(0, INPUT_PULLUP);
  // pinMode(18, OUTPUT);

  // digitalWrite(18, LOW);
  digitalWrite(25, HIGH);
  delay(2000);
  //two hardware serials for pico, serial1 (gpio1) for printing in monitor, serial (gpio0) for openmv (can set pins, openmv side uart)
  //ask if can use software serial (serial 2) for bluetooth comms with other bot????? or? 

  us_setup();
  i2c_setup();

}

void loop() {
  digitalWrite(25, LOW);
  
  send_us();
  upd_coords();
  // print_us();

  // Serial.println(in_bcz());
  // Serial.print("intensity: ");
  // Serial.print(getirfront(9));
  // Serial.print(" angle: ");
  // Serial.println(interpolate());
  // Serial.println(temt_boundary());
  Serial.println(compass_angle());



  double angle, speed, rotation; 
  
  read_temts();

  if (digitalRead(22) == 0) { //switch 
    drive2(0.0, 0.0, 0.0);

  } else if (digitalRead(0) == 0) { //switch 
    drive2(0.0, 0.0, 0.0);
  //  digitalWrite(25, LOW);
  
    
  }
  else {
    // digitalWrite(25, HIGH);
    // Serial.println(check_wall()); // heree

    // if (check_wall()) {
    //   // drive2(temt_boundary(), compass_error(), 0.2);
    //   angle = temt_boundary();
    //   speed = 0.3;
    //   digitalWrite(25, HIGH);
    //   Serial.println(temt_boundary());
    // }
    // else {
    // Serial.println("SWITCH ON");
    bool new_bc_state = in_bcz();
    if (bc_state == false && new_bc_state == true){
      start_aim = millis();
    } 
    bc_state = new_bc_state;
      if (bc_state){
        
        angle = move_to_point(get_x_pos(), get_y_pos(), 900, 2430);
        long long timesinceaim = (double) (millis() - start_aim);
        speed = 0.30 + 0.0005 * max(timesinceaim, 500);
        if (timesinceaim > 300) speed += 0.0002 * (timesinceaim - 300);
        digitalWrite(25, HIGH);

        rotation = face_goal(read_camera());
        // rotation = compass_error();

        if (timesinceaim > 200 && millis() - timeoflastkick > 1000) { //kicker add if x pos greater
          Serial2.write(1);
          timeoflastkick = millis();
        }


      }

      else if (getirfront(9) < 10 && getirback(9) < 10) {
        angle = move_to_point(get_x_pos(), get_y_pos(), 910.0, 1215.0);
        speed = 0.3;
        rotation = compass_error();
        digitalWrite(25, LOW);
      }

      else{
        digitalWrite(25, LOW);
        angle = orbit_ball();
        speed = 0.3;
        rotation = compass_error();
      }

      // digitalWrite(25, LOW);
    // }
    slow_down(speed, angle, get_x_pos(), get_y_pos());
    upd_conf(get_slowdown_angle(), get_slowdown_speed());
    // upd_conf(angle, speed);
    drive2(get_conf_angle(), rotation, get_conf_speed());
  }


  // drive(90.0, 0.2);
  // delay(2000);
  // drive(180.0, 0.2);
  // delay(2000);
  // drive(270.0, 0.2);
  // delay(2000);
  // drive(0.0, 0.2);
  // delay(2000);

  // run_motor(0.2,NE_speed, SE_dir, NE_cw);
  // delay(2000);
  // run_motor(0.2,NE_speed, SE_dir, -NE_cw);
  // delay(2000);
  // run_motor(0.0 ,NE_speed, SE_dir, NE_cw);

  
  // run_motor(0.2,SE_speed, NE_dir, SE_cw);
  // delay(2000);
  // run_motor(0.2,SE_speed, NE_dir, -SE_cw);
  // delay(2000);
  // run_motor(0.0 ,SE_speed, NE_dir, SE_cw);
  
   
  // run_motor(0.2,SW_speed, SW_dir, SW_cw);
  // delay(2000);
  // run_motor(0.2,SW_speed, SW_dir, -SW_cw);
  // delay(2000);
  // run_motor(0.0 ,SW_speed, SW_dir, SW_cw);

  
  // run_motor(0.2,NW_speed, NW_dir, NW_cw);
  // delay(2000);
  // run_motor(0.2,NW_speed, NW_dir, -NW_cw);
  // delay(2000);
  // run_motor(0.0 ,NW_speed, NW_dir, NW_cw);



//pure rotation is if bot facing back, turn infront then start, see 010425
}
