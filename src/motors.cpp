#include <config.h>
#include <rcj_vision26.h>
#include <Arduino.h>

void setup () {

}

void loop () {
  // run_motor(100, SE_speed, SE_dir, SE_cw);
  // delay(2000);
  // run_motor(100, NE_speed, NE_dir, NE_cw);
  // delay(2000);
  // run_motor(100, SW_speed, SW_dir, SW_cw);
  // delay(2000);
  // run_motor(100, NW_speed, NW_dir, NW_cw);
  // delay(2000);

  drive(90.0, 100.0);
  delay(2000);

}