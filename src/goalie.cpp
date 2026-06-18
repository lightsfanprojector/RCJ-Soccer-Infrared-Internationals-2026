#include <config.h>
#include <rcj_vision26.h>
#include <Arduino.h>


long long start_aim;

bool bc_state = false;

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

  //two hardware serials for pico, serial1 (gpio1) for printing in monitor, serial (gpio0) for openmv (can set pins, openmv side uart)
  //ask if can use software serial (serial 2) for bluetooth comms with other bot????? or? 


  // camera to see goal (angle to center / not blocked place??), then do aiming towards goal in camera instead of coordinate
  //to do: 
  //make ir and compass work 
  //do ir graph for ballcoord
  // figure out what ballcoord is for
  //temts!!!!IMPORTANT????? how to do???????
  //figure out comms (camera and bt)
  //do camera stuff 

  // what even is the code logic????? strategy????




  //if time: cool cadence goalie arc
  us_setup();
  i2c_setup();

}

void loop() {
  
  send_us();
  upd_coords();
  print_us();
  // Serial.print(get_x_conf());
  // Serial.print(" ");
  // Serial.println(get_y_conf());

  // Serial.print("intensity: ");
  // Serial.print(getirfront(9));
  // Serial.print(" angle: ");
  // Serial.println(interpolate());

  double angle, speed, rightxcoord, leftxcoord; 
  bool goalie = 1; 
  
  // Serial.println(getirfront(9));

  read_temts();

  if (getirfront(9) > 85){
    goalie = 0;
  } else goalie = 1;

    if (goalie == 1){
        digitalWrite(25, HIGH);
        if (interpolate() < 180) {
        rightxcoord = 910.0 + sin(dtor(interpolate())) * 360; //scaled based on location of ball 
        angle = move_to_point(get_x_pos(), get_y_pos(), rightxcoord, 430.0);
        speed = get_movetopoint_speed();
    }

        else {
            leftxcoord = 910.0 + sin(dtor(interpolate())) * 360;
            angle = move_to_point(get_x_pos(), get_y_pos(), leftxcoord, 430.0);
            speed = get_movetopoint_speed();
        }

        if (getirfront(9) < 25 && getirback(9) < 25) {  //if cannot see ball, move to centre of goal line
        // digitalWrite(25, LOW);
        angle = move_to_point(get_x_pos(), get_y_pos(), 910.0, 430.0);
        speed = get_movetopoint_speed();
        }
    }

    if (goalie == 0){

      digitalWrite(25, LOW);

        // Serial.println(check_wall());

        // if (check_wall()) {
        // // drive2(temt_boundary(), compass_error(), 0.2);
        // angle = temt_boundary();
        // speed = 0.2;
        // digitalWrite(25, HIGH);
        // Serial.println(temt_boundary());
        // }

        // else {
        // if (in_bcz()){
        //     angle = move_to_point(get_x_pos(), get_y_pos(), 900, 2430);
        //     speed = 0.4; 
        // }

        // else if (getirfront(9) < 30 && getirback(9) < 30){ //if cannot see ball, move to middle
        //     angle = move_to_point(get_x_pos(), get_y_pos(), 910.0, 1215.0);
        //     speed = 0.3;
        // }

        // else{ //if not in bcz, orbit ball
        //     angle = orbit_ball();
        //     speed = 0.4;
        // }

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

      }

      else if (getirfront(9) < 10 && getirback(9) < 10) {
        angle = move_to_point(get_x_pos(), get_y_pos(), 910.0, 1215.0);
        speed = 0.3;
        digitalWrite(25, LOW);
      }

      else{
        digitalWrite(25, LOW);
        angle = orbit_ball();
        speed = 0.45;
      }
        digitalWrite(25, LOW);
        // }
    }
  // }


    if (digitalRead(22) == 0) {
    drive2(0.0, 0.0, 0.0);
    } 
    else {
      slow_down(speed, angle, get_x_pos(), get_y_pos());
      upd_conf(get_slowdown_angle(), get_slowdown_speed());
      drive2(get_conf_angle(), compass_error(), get_conf_speed());
      // drive2(angle, compass_error(), speed);
    }
}

//pure rotation is if bot facing back, turn infront then start, see 010425

