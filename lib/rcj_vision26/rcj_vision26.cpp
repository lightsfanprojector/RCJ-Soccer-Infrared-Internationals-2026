#include <rcj_vision26.h>
#include <config.h>

long long ultrasonic_front_start = 0;
long long ultrasonic_back_start = 0;
long long ultrasonic_left_start = 0;
long long ultrasonic_right_start = 0;

double ultrasonic_front_distance;
double ultrasonic_back_distance;
double ultrasonic_left_distance;
double ultrasonic_right_distance;

long long aim_time = 0;
long long lastRead;

double x_pos, y_pos, lb_x, ub_x, lb_y, ub_y, x_conf, y_conf, ball_x, ball_y;
double goalie_x, goalie_y;
double sspeed1, angle1, sspeed2, angle2, sspeed3, sspeed4;

int prev_ball = 0;

// TwoWire Wire(BACK_LIDAR_SDA, BACK_LIDAR_SCL); // 
// TwoWire Wire1(FRONT_LIDAR_SDA, FRONT_LIDAR_SCL); //grnd 5v sda scl

void run_motor(double sspeed, int sspeedpin, int dirpin, int cw) { // speed is value from -1 to 1
  sspeed *= cw;
  if (sspeed > 0) {
    analogWrite(sspeedpin,sspeed*255); // speed
    analogWrite(dirpin,255); // dir
  }
  else {
    analogWrite(sspeedpin,fabs(sspeed)*255); // speed
    analogWrite(dirpin,0);//dir
  }
}

double dtor(double angle) {
  return angle/180.0*PI;
}

void drive(double angle, double sspeed) {
  angle = angle - 45.0;
  // 0 -> -45 -> 315
  if (angle < 0.0) {
    angle = 360 - fabs(angle);
  }
  double opp = sin(dtor(angle)) * sspeed;
  double adj = cos(dtor(angle)) * sspeed;
  run_motor(opp, NE_speed, NE_dir, NE_cw);
  run_motor(opp, SW_speed, SW_dir, SW_cw);
  run_motor(adj, SE_speed, SE_dir, SE_cw);
  run_motor(adj, NW_speed, NW_dir, NW_cw);
}

void drive2(double angle, double rotation, double speedd) {
  angle -= 45.0;
  if (angle < 0.0) angle = 360.0 - fabs(angle);
  speedd = constrain(speedd, -1.0, 1.0);
  rotation = constrain(rotation, -1.0, 1.0);
  double opp = sin(dtor(angle))*speedd;
  double adj = cos(dtor(angle))*speedd;
  double new_opp = opp - 2 * fabs(rotation) * opp;
  double new_adj = adj - 2 * fabs(rotation) * adj;
  // NE and SW
  if (angle > 0.0 && angle < 180.0) {
    // NE is CW, SW is CCW
    if (rotation > 0.0) {
      // turn clockwise
      // slow down sw
      run_motor(new_opp, SW_speed, SW_dir, SW_cw);
      run_motor(opp, NE_speed, NE_dir, NE_cw);
      // is that all? possibly
    }
    else {
      run_motor(new_opp, NE_speed, NE_dir, NE_cw);
      run_motor(opp, SW_speed, SW_dir, SW_cw);
    }
  }
  else {
    // swapppp 
    if (rotation > 0.0) {
      // turn clockwise
      run_motor(new_opp, NE_speed, NE_dir, NE_cw);
      run_motor(opp, SW_speed, SW_dir, SW_cw);
    }
    else {
      run_motor(new_opp, SW_speed, SW_dir, SW_cw);
      run_motor(opp, NE_speed, NE_dir, NE_cw);
    }
  }
  if (angle > 90.0 && angle < 270.0) {
    // SE is CW, NW is CCW
    if (rotation > 0.0) {
      // turn clockwise, slow down CCW
      run_motor(new_adj, NW_speed, NW_dir, NW_cw);
      run_motor(adj, SE_speed, SE_dir, SE_cw);
    }
    else {
      run_motor(new_adj, SE_speed, SE_dir, SE_cw);
      run_motor(adj, NW_speed, NW_dir, NW_cw);
    }
  }
  else {
    if (rotation > 0.0) {
      run_motor(new_adj, SE_speed, SE_dir, SE_cw);
      run_motor(adj, NW_speed, NW_dir, NW_cw);
    }
    else {
      run_motor(new_adj, NW_speed, NW_dir, NW_cw);
      run_motor(adj, SE_speed, SE_dir, SE_cw);
    }
  }
}
double compass_angle() {
    Wire1.beginTransmission(0x3B);
    Wire1.write(0x3B); 
    Wire1.endTransmission(false); 

    // 2. Request 2 bytes
    if (Wire1.requestFrom(0x3B, 2) == 2) {
        byte highByte = Wire1.read();
        byte lowByte = Wire1.read();

        // 3. Combine bytes correctly: (High << 8) | Low
        int16_t combinedValue = (highByte << 8) | lowByte;

        return (double)combinedValue; // Return the actual value
    }

    return 0.0; // Return a default value if the sensor fails to respond
} // <--- This was the missing bracket


//cameraa angleeeeeee hereeee
//error = compass - target (goal ang from cam) probably????/ 
//angle use pixels, distance use graph aain with pixels yay g
//angle map to 255, send, convert back 
//distance for movetopoint 
double compass_error() { //paramter???
  double ang = compass_angle(); //error, rn assumes its 0, constrain later
  if (ang > 180.0) ang -= 360.0;
  ang /= -45.0;
  return ang; 
}

double face_goal(double goal_angle) {
  double ang = compass_angle() - goal_angle;
  if (ang > 180.0) ang -= 360.0;
  if (ang < -180.0) ang += 360.0;
  ang /= -45.0;
  return ang; 
}

double get_basic_angle(double ang) {
  if (ang < 90.0) return ang;
  else if (ang < 180.0) return 180.0 - ang;
  else if (ang < 270.0) return ang - 180.0;
  else return 360.0 - ang;
}

int getirfront(byte commandd) {
  Wire1.beginTransmission(lidar_addr);
  Wire1.write(commandd);
  Wire1.endTransmission();
  Wire1.requestFrom(lidar_addr,1);
  while (Wire1.available() < 1);
  return (Wire1.read());
}

int getirback(byte commandd) {
  Wire.beginTransmission(lidar_addr);
  Wire.write(commandd); 
  Wire.endTransmission();
  Wire.requestFrom(lidar_addr,1);
  while (Wire.available() < 1);
  return (Wire.read());
}


int ir_channels[] = {0, 5, 6, 7, 1, 2, 3, 4};
int front_ir_ang[] = {0, 360, 330, 300, 270, 450, 420, 390};
int back_ir_ang[] = {0, 180, 150, 120, 90, 270, 240, 210};

int interpolate() {
  int maxfrontval = getirfront(9);
  int maxfrontport = getirfront(8);
  int maxbackval = getirback(9);
  int maxbackport = getirback(8);
  int leftval, rightval;
  int leftportang, rightportang, maxportang;
  if (maxfrontval > maxbackval) {
    if (maxfrontport == 1) {
      // edgy
      leftval = getirfront(3);
      rightval = getirback(5);
      maxportang = 90;
      rightportang = 90;
      leftportang = 60;
    }
    else if (maxfrontport == 7) {
      // also edgy
      leftval = getirback(4); // i think
      rightval = getirfront(6); // yessies
      maxportang = 270;
      leftportang = 270;
      rightportang = 300;
    }
    else {
      leftval = getirfront(ir_channels[maxfrontport-1]);
      rightval = getirfront(ir_channels[maxfrontport+1]);
      maxportang = front_ir_ang[ir_channels[maxfrontport]];
      leftportang = front_ir_ang[ir_channels[maxfrontport-1]];
      rightportang = front_ir_ang[ir_channels[maxfrontport+1]];
    }
  }
  else {
    // back is maximum
    // edgies
    if (maxbackport == 1) {
      rightval = getirfront(5); // i think
      leftval = getirback(3); // yessies
      maxportang = 270;
      leftportang = 240;
      rightportang = 270;
    }
    else if (maxbackport == 7) {
      leftval = getirfront(4);
      rightval = getirback(6);
      maxportang = 90;
      rightportang = 120;
      leftportang = 90;
    }
    else {
      leftval = getirback(ir_channels[maxbackport-1]);
      rightval = getirback(ir_channels[maxbackport+1]);
      maxportang = back_ir_ang[ir_channels[maxbackport]];
      leftportang = back_ir_ang[ir_channels[maxbackport-1]];
      rightportang = back_ir_ang[ir_channels[maxbackport+1]];
    }
  }
  int leftdiff, rightdiff; 
  if (maxfrontval > maxbackval) {
    rightdiff = maxfrontval - rightval;
    leftdiff = maxfrontval - leftval;
  }
  else {
    rightdiff = maxbackval - rightval;
    leftdiff = maxbackval - leftval;
  }
  int new_ang, const_val;
  if (leftdiff < rightdiff) { // closer to left
    const_val = abs(maxportang - leftportang) / 2;
    new_ang = (maxportang + (const_val - int(((double)leftdiff/(double)rightdiff) * (double)const_val))) % 360;
  }
  else {
    const_val = abs(maxportang - rightportang) / 2;
    new_ang = (maxportang - (const_val - int(((double)rightdiff/(double)leftdiff) * (double)const_val))) % 360;
  }
  return new_ang;
}

// int ir_channels[] = {0, 5, 6, 7, 1, 2, 3, 4}; 
// int front_ir_ang[] = {0, 360, 330, 300, 270, 450, 420, 390};
// int back_ir_ang[] = {0, 180, 150, 120, 90, 270, 240, 210};

// int interpolate() {
//   int maxfrontval = getirfront(9);
//   int maxfrontport = getirfront(8);
//   int maxbackval = getirback(9);
//   int maxbackport = getirback(8);
//   int leftval, rightval;
//   int leftportang, rightportang, maxportang;
//   bool side = false;
//   if (maxfrontval > maxbackval) {
//     // Serial.println("front");
//     if (maxfrontport == 1) {
//       // edgy
//       side = true;
//       leftval = getirfront(4);
//       rightval = getirback(6); //flipped 
//       maxportang = 90;
//       rightportang = 90;
//       leftportang = 60;
//     }
//     else if (maxfrontport == 7) {
//       // also edgy
//       side = true;
//       leftval = getirback(5); // i think 4
//       rightval = getirfront(3); // yessies //?????what? 6
//       maxportang = 270;
//       leftportang = 270;
//       rightportang = 300;
//     }
//     else {
//       leftval = getirfront(ir_channels[maxfrontport+1]);
//       rightval = getirfront(ir_channels[maxfrontport-1]);
//       maxportang = front_ir_ang[ir_channels[maxfrontport]];
//       leftportang = front_ir_ang[ir_channels[maxfrontport+1]];
//       rightportang = front_ir_ang[ir_channels[maxfrontport-1]]; //flipp
//     }
//   }
//   else {
//     // Serial.println("back");
//     // back is maximum
//     // edgies
//     if (maxbackport == 1) {
//       side = true;
//       rightval = getirfront(4); // i think
//       leftval = getirback(6); // yessies
//       maxportang = 270;
//       leftportang = 240;
//       rightportang = 270;
//     }
//     else if (maxbackport == 7) {
//       side = true;
//       leftval = getirfront(3);
//       rightval = getirback(5);
//       maxportang = 90;
//       rightportang = 120;
//       leftportang = 90;
//     }
//     else {
//       leftval = getirback(ir_channels[maxbackport+1]);
//       rightval = getirback(ir_channels[maxbackport-1]);
//       maxportang = back_ir_ang[ir_channels[maxbackport]];
//       leftportang = back_ir_ang[ir_channels[maxbackport+1]];
//       rightportang = back_ir_ang[ir_channels[maxbackport-1]]; //flipped
//     }
//   }
//   double leftdiff, rightdiff;
//   double ratio, a, b, x, y; 
//   int new_ang;
//   //tune this stuff
//   int idkmanthresh = 3;
//   int h = 10;
//   int w = 7;
//   bool left = false;

//   if (maxfrontval > maxbackval) {
//     rightdiff = maxfrontval - rightval;
//     leftdiff = maxfrontval - leftval;
//   }
//   else {
//     rightdiff = maxbackval - rightval;
//     leftdiff = maxbackval - leftval;
//   }
//   // Serial.print("left diff: ");
//   // Serial.println(leftdiff);
//   // Serial.print("right diff:");
//   // Serial.println(rightdiff);

//   if (leftdiff == 0) leftdiff = 1;
//   if (rightdiff == 0) rightdiff = 1;

//   if (maxfrontval > maxbackval && maxfrontport == 7) {
//     // Serial.println("front left");
//     ratio = leftdiff / rightdiff;
//     left = true;
    
//     a = getirfront(4) * -1.04 + 82.8;
//     b = getirback(5) * -1.0 + 65.0;
//   }

//   else if (maxfrontval > maxbackval && maxfrontport == 1) {
//     ratio = rightdiff / leftdiff;
//     // Serial.println("front right");
//     a = getirfront(5) * -0.444 + 56.7; 
//     b = getirback(4) * -0.8 + 58;
//   }

//   else if (maxbackval >= maxfrontval && maxbackport == 1) {
//     // Serial.println("back left");
//     ratio = leftdiff / rightdiff;
//     left = true;
//     a = getirfront(4) * -1.04 + 82.8; 
//     b = getirback(5) * -1.0 + 65.0;
//   }

//   else if (maxbackval >= maxfrontval && maxbackport == 7) {
//     ratio = rightdiff / leftdiff;
//     // Serial.println("back right");
//     a = getirfront(5) * -0.444 + 56.7; 
//     b = getirback(4) * -0.8 + 58;
//   }
    
//   if (ratio > idkmanthresh){
//     side = false;
//   }

//   // Serial.print(ratio);
//   // Serial.print(" ");
//   // Serial.println(side);

//   if (side) {
//     int angle; 
//     angle = acos((b*b + h*h - a*a)/ (2*b*h)); 
//     // x = b * sin(angle) + w/2;
//     y = b * cos(angle) - h/2;
//     if (left){
//       x = b * sin(angle) + w/2;
//       new_ang = int(90.0 - (atan2(y, x) * 180.0 / PI)) % 360; //270+ 
//     }
//     else {
//       x = b * sin(angle) + w/2;
//       new_ang = int(90.0 - (atan2(y, x) * 180.0 / PI)) % 360;
//     }
//   }

//   else {
//   int const_val;
//   if (leftdiff < rightdiff) { // closer to left
//     const_val = abs(maxportang - leftportang) / 2;
//     new_ang = (maxportang - (const_val - int(((double)leftdiff/(double)rightdiff) * (double)const_val))) % 360;
//   } //first sign flipped
//   else {
//     const_val = abs(maxportang - rightportang) / 2;
//     new_ang = (maxportang + (const_val - int(((double)rightdiff/(double)leftdiff) * (double)const_val))) % 360;
//   } //first sign fipped
//   }
//   // if (new_ang < 20){
//   //   new_ang -= 15; 
//   //   if (new_ang < 0) new_ang += 360;
//   // }
//   return new_ang;
// }

int orbit_ball() {
  double ang = (double)interpolate();
//  int maxval = max(getirfront(9), getirback(9));
  int frontmax = getirfront(9);
  int backmax = getirback(9);
  double closeness; 
  if (frontmax > backmax) closeness = (double)frontmax / FRONT_LIDAR_MAX; // measured max so yay? :cD
  else closeness = (double)backmax / BACK_LIDAR_MAX;

  if (ang > BCZ_ANGLE_LEFT) {
    // ???
    // Serial.println("hehe");
    ang = 360.0 - (pow(1.0 - closeness, 1) * (360.0 - ang)); //0.4
    sspeed4 = 0.3 + 0.7 * pow(1.0 - closeness, 2.0); //used to be 1.2 here
  }
  else if (ang < BCZ_ANGLE_RIGHT) {
    // ???
    // Serial.println("hehe2");
    ang = pow(1.0 - closeness, 1) * ang; //0.4
    sspeed4 = 0.3 + 0.7 * pow(1.0 - closeness, 2.0); //used to be 1.2
  }

  // if (ang > BCZ_ANGLE_LEFT || ang < BCZ_ANGLE_RIGHT){
  //   ang = ang;
  // }

  else if (ang < 180.0) {
    // ???
    // Serial.println("heheh3");
    ang += pow(closeness, 0.90) * 85.0; //used t be .4, 6
    double ang_err = ang;
    if (ang_err > 180.0) ang_err -= 360.0;
    ang_err /= 180.0;
    sspeed4 = 0.1 + 0.45 * pow(1.0 - closeness, 3.0) + 0.45 * pow(fabs(ang_err), 2.0); //2.9 used to be 2.0
  }
  else {
    // ??? 
    // Serial.println("he4");
    ang -= pow(closeness, 0.90) * 90.0; //used t be .4, 6
    double ang_err = ang;
    if (ang_err > 180.0) ang_err -= 360.0;
    ang_err /= 180.0;
    sspeed4 = 0.1 + 0.45 * pow(1.0 - closeness, 3.0) + 0.45 * pow(fabs(ang_err), 2.0);
  }
  if (sspeed4 > 0.45) sspeed4 = 0.45;

  return ang;
}

void i2c_setup() {
  Wire.setSCL(BACK_LIDAR_SCL); 
  Wire.setSDA(BACK_LIDAR_SDA); 
  Wire.begin();
  Wire.beginTransmission(lidar_addr);
  Wire.write(14);
  Wire.endTransmission();

  Wire1.setSCL(FRONT_LIDAR_SCL);
  Wire1.setSDA(FRONT_LIDAR_SDA);
  Wire1.begin();
  Wire1.beginTransmission(lidar_addr);
  Wire1.write(14);
  Wire1.endTransmission();
  // Wire1.setTimeout(1000);
}

void ultrasonicPulse(){
    // Must send HIGH for 10 microseconds, so we just wait
    // digitalWrite(ULTRASONIC_PIN_OUTPUT, HIGH);
    digitalWrite(US_F_O, HIGH);
    digitalWrite(US_B_O, HIGH);
    digitalWrite(US_L_O, HIGH);
    digitalWrite(US_R_O, HIGH);
    delayMicroseconds(10);
    digitalWrite(US_F_O, LOW);
    digitalWrite(US_B_O, LOW);
    digitalWrite(US_L_O, LOW);
    digitalWrite(US_R_O, LOW);
    ultrasonic_front_start = micros();
    ultrasonic_back_start = micros();
    ultrasonic_left_start = micros();
    ultrasonic_right_start = micros();
}

void ultrasonicFrontEcho() {
    if(ultrasonic_front_start != 0){
        // calculate the distance by measuring how long it took to return the sound
        // The speed of sound is 343 m/s and we need half the time it took (since
        // the sound has to travel towards the object _AND_ back). So a single echo does
        // 1/(343/2) = 0.005831 seconds per meter
        ultrasonic_front_distance = ((micros() - ultrasonic_front_start)* F_m + F_c) * 10; 
        ultrasonic_front_start = 0;
        // ultrasonicPulse();
    }
}

void ultrasonicBackEcho() {
    if(ultrasonic_back_start != 0){
        // calculate the distance by measuring how long it took to return the sound
        // The speed of sound is 343 m/s and we need half the time it took (since
        // the sound has to travel towards the object _AND_ back). So a single echo does
        // 1/(343/2) = 0.005831 seconds per meter
        ultrasonic_back_distance = ((micros() - ultrasonic_back_start) * B_m + B_c) * 10;

        ultrasonic_back_start = 0;
        // ultrasonicPulse();
    }
}

void ultrasonicLeftEcho() {
    if(ultrasonic_left_start != 0){
        // calculate the distance by measuring how long it took to return the sound
        // The speed of sound is 343 m/s and we need half the time it took (since
        // the sound has to travel towards the object _AND_ back). So a single echo does
        // 1/(343/2) = 0.005831 seconds per meter
        ultrasonic_left_distance = ((micros() - ultrasonic_left_start) * L_m + L_c) * 10; //10
        ultrasonic_left_start = 0;
        // ultrasonicPulse();
    }
}

void ultrasonicRightEcho() {
    if(ultrasonic_right_start != 0){
        // calculate the distance by measuring how long it took to return the sound
        // The speed of sound is 343 m/s and we need half the time it took (since
        // the sound has to travel towards the object _AND_ back). So a single echo does
        // 1/(343/2) = 0.005831 seconds per meter
        ultrasonic_right_distance = ((micros() - ultrasonic_right_start)* R_m + R_c) * 10;
        ultrasonic_right_start = 0;
        // ultrasonicPulse();
    }
}

void us_setup() {
    pinMode(US_F_I, INPUT_PULLUP);
    pinMode(US_F_O, OUTPUT);
    pinMode(US_B_I, INPUT_PULLUP);
    pinMode(US_B_O, OUTPUT);
    pinMode(US_L_I, INPUT_PULLUP);
    pinMode(US_L_O, OUTPUT);
    pinMode(US_R_I, INPUT_PULLUP);
    pinMode(US_R_O, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(US_F_I), ultrasonicFrontEcho, FALLING);
    attachInterrupt(digitalPinToInterrupt(US_B_I), ultrasonicBackEcho, FALLING);
    attachInterrupt(digitalPinToInterrupt(US_L_I), ultrasonicLeftEcho, FALLING);
    attachInterrupt(digitalPinToInterrupt(US_R_I), ultrasonicRightEcho, FALLING);
    ultrasonicPulse();
}

void send_us() {
    if (millis() - lastRead > ULTRASONIC_TIMER_US) {
      lastRead = millis();
      ultrasonicPulse();
    }
}

void print_us() {
    Serial.print("FRONT: ");
    Serial.print(ultrasonic_front_distance);

    Serial.print(" BACK: ");
    Serial.print(ultrasonic_back_distance);

    Serial.print("  LEFT: ");
    Serial.print(ultrasonic_left_distance);

    Serial.print("  RIGHT: ");
    Serial.print(ultrasonic_right_distance);

    Serial.print(" X COR: ");
    Serial.print(x_pos);
    Serial.print(" Y COR: ");
    Serial.print(y_pos);

    Serial.println();
}

void upd_coords() {
    double comp_ang = compass_angle();
    double ang = get_basic_angle(comp_ang);

    // ang = 0.0;

    double left, right, front, back;
    left = ultrasonic_left_distance * cos(dtor(ang));
    right = ultrasonic_right_distance * cos(dtor(ang));
    lb_x = left; // in mm
    ub_x = FIELD_X - right;
    

    if (ub_x < lb_x) lb_x = ub_x; // left sensor is reliable right is not

    x_pos = (lb_x + ub_x) /  2.0;

    if (left < 300.0) x_pos = lb_x;
    else if (right < 300.0) x_pos = ub_x;

    front = ultrasonic_front_distance * cos(dtor(ang));
    back = ultrasonic_back_distance * cos(dtor(ang));

    lb_y = back;
    ub_y = FIELD_Y - front;
    if (610.0 < x_pos < 1210.0) {
      lb_y += 70.0;
      ub_y -= 70.0;
    }

    y_pos = (lb_y + ub_y) / 2.0;

    if (lb_y > ub_y) lb_y = ub_y; // back sensor is not reliable
    // if (lb_y < 0.0) lb_y = 0.0;
    // if (cadence) {
    //   ub_y = lb_y;
      // y_pos = (lb_y + ub_y) / 2.0;
    // }

    if (back < 550.0) y_pos = lb_y;
    else if (front < 550.0) y_pos = ub_y; 


}
void upd_conf(double angle, double sspeed) {
    double x_speed = sspeed * sin(dtor(angle));
    double y_speed = sspeed * cos(dtor(angle)); 

    x_conf = pow(1.0 - fabs(lb_x - ub_x) / FIELD_X, 2.0);
    y_conf = pow(1.0 - fabs(lb_y - ub_y) / FIELD_Y, 2.0);

    if (x_conf > 1.0) x_conf = 1.0;
    if (x_conf < 0.0) x_conf = 0.0;
    if (y_conf > 1.0) y_conf = 1.0;
    if (y_conf < 0.0) y_conf = 0.0;

    if (x_speed > x_conf) {
      x_speed = x_conf;
    }
    else if (x_speed < -x_conf){
      x_speed = -x_conf;
    }
    if (y_speed > y_conf) {
      y_speed = y_conf;
    }
    else if (y_speed < -y_conf) {
      y_speed = -y_conf;
    }

    angle2 = atan2(x_speed, y_speed) * 180.0 / PI;
    if (angle2<0.0) angle2 += 360.0;
    sspeed2 = sqrt(x_speed * x_speed + y_speed * y_speed);
}

double get_x_conf() {
  return x_conf;
}

double get_y_conf() {
  return y_conf;
}

double move_to_point(int cur_x, int cur_y, int target_x, int target_y) {
    double xDiff = target_x - cur_x; //mm
    double yDiff = target_y - cur_y; //mm

    double angle = 180.0 / PI * (atan2(xDiff, yDiff));
    double distance = sqrt((xDiff)*(xDiff) + (yDiff)* (yDiff));
    if (distance > 500.0) distance = 500.0;
    sspeed3 = 0.33 * (0.5* pow(distance/500.0, 0.50) + 0.5*pow(getirfront(9)/FRONT_LIDAR_MAX, 0.5)); //max front ir intensity, change in config file
    // Serial.print(xDiff);
    // Serial.print(" ");
    // Serial.print(yDiff);
    // Serial.print(" dist ");
    // Serial.print(distance);
    // Serial.print(" speedd3 "); 
    // Serial.print(sspeed3);
    return angle;
}

bool in_bcz() {
    if ((BCZ_ANGLE_LEFT < interpolate() || BCZ_ANGLE_RIGHT > interpolate()) && getirfront(9) > BALL_INTENSITY) {
      Serial.print(" sdjflksdjfls ");
      // prev_ball = 1;
      // long long start_aim 
      aim_time = millis();
      return 1;
    }
    else if (millis() - aim_time < 500) return 1;
    else{return 0;}
    
    // if (prev_ball && 355.0 < interpolate() || 10.0 > interpolate() && getirfront(9) > 130){
    //   Serial.print(" dsfjslsdkjfdkslj ");
    //   prev_ball = 1;
    //   // aim_time = millis();
    //   return 1;
    // }
    // && analogRead(BCZ_PIN) < BCZ_THRESH
    //prev_ball &&

    // prev_ball = 0;
    if (millis() - aim_time < 500) return 1;
    if (analogRead(BCZ_PIN) < BCZ_THRESH) prev_ball = 1; 
    // else prev_ball = 0;
    return 0;
    if (millis() - aim_time > 300) return 0; 
    return 1; 
    // if (analogRead(BCZ_PIN) > BCZ_THRESH) return 1;
    // return 0;
}

double get_x_pos() {
  return x_pos;
}

double get_y_pos() {
  return y_pos;
}

long long get_aim_time() {
  return aim_time;
}

void slow_down(double sspeed, double angle, double x_pos, double y_pos) {
  double x_speed = sspeed * sin(dtor(angle));
  double y_speed = sspeed * cos(dtor(angle)); 

  double dist; 
  double maximum_speed; 
  if (angle <= 180.0) {
    dist = FIELD_X-230.0-x_pos;
    maximum_speed = dist/FIELD_X;
  } 
  else {
    dist = x_pos - 230.0; 
    maximum_speed = dist/FIELD_X;
  }
  if (x_speed > maximum_speed){
    x_speed = maximum_speed;
  }
  else if (x_speed < -maximum_speed){
    x_speed = -maximum_speed;
  }

  if (angle <90.0 || angle >270.0) {
    dist = FIELD_Y-370.0-y_pos;
    maximum_speed = dist/FIELD_Y;
  } 
  else {
    dist = y_pos - 370.0; 
    maximum_speed = dist/FIELD_Y;
  }

  if (y_speed > maximum_speed) {
    y_speed = maximum_speed;
  }

  else if (y_speed < -maximum_speed){
    y_speed = -maximum_speed;
  }

  angle1 = atan2(x_speed, y_speed) * 180.0 / PI;
  if (angle1<0.0) angle1 += 360.0;
  sspeed1 = pow(sqrt(x_speed * x_speed + y_speed * y_speed), 0.75);
}

double get_slowdown_speed() {
  return sspeed1;
}

double get_slowdown_angle() {
  return angle1;
}

double get_conf_speed() {
  return sspeed2;
}

double get_conf_angle() {
  return angle2;
}

double get_movetopoint_speed() {
  return sspeed3;
}

double get_orbit_speed(){
  return sspeed4;
}

void pureRotation() {
  double comp_ang = compass_angle();
  double rotation;
  if (comp_ang > 180.0) rotation = 1.0;
  else rotation = -1.0;
  drive2(0.0, rotation, 0.1);
}

///////////////////////////////////////////////////////////////new!!!! 

void upd_ballcoords(){
  double x_pos = get_x_pos();
  double y_pos = get_y_pos();
  double angle = dtor(interpolate());
  double magnitude, real_magnitude; 

  if (angle < PI/2 || angle > 3*PI/2) { //ball in front
    magnitude = getirfront(9);
    // real_magnitude = -0.753 * magnitude + 97.2;
    real_magnitude = -1.38 * magnitude + 180.0;
  }
  else { //ball behind 
    magnitude = getirback(9);
    real_magnitude = -0.706 * magnitude + 75.8;
  } 



  double x_dist = real_magnitude * sin(angle); 
  double y_dist = real_magnitude * cos(angle);

  ball_x = x_pos + x_dist;
  ball_y = y_pos + y_dist;

  Serial.print("x_dist:");
  Serial.print(x_dist);
  Serial.print(" y_dist:");
  Serial.println(y_dist);
 
}

double get_ball_x(){ //define in other file
  return ball_x;
}

double get_ball_y(){ //define in other file
  return ball_y;
}


// void print_ball(){
//   Serial.print("x_dist:");
//   Serial.print(x_dist);
//   Serial.print("y")
// }

void goalie_coords(int bx, int by){ //?????? 
  double bl = fmod(atan2(by - 250, bx - 610) * 180.0 / PI + 180.0, 180.0);
  double br = fmod(atan2(by - 250, bx - 1210) * 180.0 / PI + 180.0, 180.0);
  double n = by;
  double m = bx; 
  double z = 0.5 * (br + bl) * (PI / 180.0);
  double T = tan(z);
  double hl = (-(sqrt(-4*m*m*T*T + 8*m*n*T + 7280*m*T*T + 2400*m*T - 4*n*n - 7280*n*T - 2400*n - 878800*T*T - 2184000*T + 2073600)) + 2*m*T*T - 2*n*T - 600*T + 1820) / (2 * (T*T + 1));
  double hr = ((sqrt(-4*m*m*T*T + 8*m*n*T + 7280*m*T*T + 2400*m*T - 4*n*n - 7280*n*T - 2400*n - 878800*T*T - 2184000*T + 2073600)) + 2*m*T*T - 2*n*T - 600*T + 1820) / (2 * (T*T + 1));
  double Nl = tan((br + bl) / 2.0 * (PI / 180.0)) * (hl - bx) + by;
  double Nr = tan((br + bl) / 2.0 * (PI / 180.0)) * (hr - bx) + by;
  double ball_angle = interpolate(); 
  if (ball_angle > 180.0){
    goalie_x = hr;
    goalie_y = Nr; 
  }
  else {
    goalie_x = hl;
    goalie_y = Nl;
  }
}

double get_goalie_x(){ //define 
  return goalie_x;
}

double get_goalie_y(){ //define 
  return goalie_y;
}

double temt_final_angle;

int temt_data[8];

uint8_t bottom_data = 0; 

void read_temts(){
    
    if (Serial2.available()){
        bottom_data = Serial2.read();
        Serial.println("reading");

        for (int i = 0; i < 8; i++){
         temt_data[i] = (bottom_data >> i) & 1; //first 7 outer ring of temts, 8th is bcz temt 
        } 

    }

//     for (int i = 0; i < 8; i++){
//         temt_data[i] = (bottom_data >> i) & 1; //first 7 outer ring of temts, 8th is bcz temt 
//     } 
}

bool check_wall(){
    int value = 0;
    temt_data[7] = 0;
    temt_data[3] = 0;
    temt_data[6] = 0;
    temt_data[2] = 0;
    
    for (int i = 0; i < 7; i++){
        value += temt_data[i];
    }

    // Serial.print(value);
    // Serial.print(" ");
    
    // for (int i = 0; i < 7; i++){
    //   Serial.print(temt_data[i]);
    //   Serial.print(" ");

    // }
    // Serial.println(" ");

    if (value == 0) return 0;
    else return 1;
    
}

double temt_boundary(){
    double boundary_angle;

    if ((temt_data[0] == 1) && (temt_data[1] == 1 || temt_data[2] == 1)){
        boundary_angle = 315.0;
        temt_final_angle = 135.0;
    }

    else if ((temt_data[3] == 1 || temt_data[4] == 1) && (temt_data[1] == 1|| temt_data[2] == 1)){
        boundary_angle = 225.0; 
        temt_final_angle = 45.0; 
    }
    
    else if ((temt_data[3] == 1 || temt_data[4] == 1) && (temt_data[5] == 1|| temt_data[6] == 1)){
        boundary_angle = 135.0; 
        temt_final_angle = 315.0; 
    }

    
    else if ((temt_data[5] == 1 || temt_data[6] == 1) && (temt_data[0] == 1)){
        boundary_angle = 45.0; 
        temt_final_angle = 225.0; 
    }

    else if (temt_data[0] == 1){
        boundary_angle = 0.0;
        temt_final_angle = 180.0;
    }

    
    else if ((temt_data[1] == 1) || (temt_data[2] == 1)){
        boundary_angle = 270.0;
        temt_final_angle = 90.0;
    }
    
    else if ((temt_data[3] == 1) || (temt_data[4] == 1)){
        boundary_angle = 180.0;
        temt_final_angle = 0.0;
    }
    
    else if ((temt_data[5] == 1) || (temt_data[6] == 1)){
        boundary_angle = 90.0;
        temt_final_angle = 270.0;
    }
    return temt_final_angle; 
  }


/////////////////////////////////////////////////////////////////// intls new!!! 

uint8_t cam_data = 0; 
double goal_angle; 

double read_camera(){
    if (Serial1.available()){
        cam_data = Serial1.read();
        Serial.println("reading");
        goal_angle = (cam_data / 255.0) * 360.0; // Map 0-255 to 0-360 degrees
        Serial.print("Goal Angle: ");
        Serial.println(goal_angle);
    }
    return goal_angle;
}

