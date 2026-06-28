#ifndef rcj_vision26_h
#define rcj_vision26_h

#include <config.h>
#include <Wire.h>

#define compass_addr 0x3B
#define lidar_addr 0x01

#define ULTRASONIC_TIMER_US 25 

void run_motor(double sspeed, int sspeedpin, int dirpin, int cw);

double dtor(double angle); //double is a floating point decimal ---> decimal + numer with 64 bits of data??? 

void drive(double angle, double speedd);
void drive2(double angle, double rotation, double speedd);

double compass_angle();
double compass_error();
double get_basic_angle(double ang);

int getirfront(byte commandd);
int getirback(byte commandd);

int interpolate();
int orbit_ball();

void i2c_setup();

void ultrasonicPulse();
void ultrasonicFrontEcho();
void ultrasonicBackEcho();
void ultrasonicLeftEcho();
void ultrasonicRightEcho();
void us_setup();
void print_us();
void send_us();

void slow_down(double sspeed, double angle, double x_pos, double y_pos);

void upd_coords();
void upd_conf(double angle, double sspeed);
double get_x_conf();
double get_y_conf();

double move_to_point(int cur_x, int cur_y, int target_x, int target_y);
double get_movetopoint_speed();

double get_x_pos();
double get_y_pos();

bool in_bcz();
long long get_aim_time();


double get_slowdown_speed();
double get_slowdown_angle();

double get_conf_angle();
double get_conf_speed();
////////////////////////////////////////////////////new!!!! 

void upd_ballcoords();
// void temt_limit(double angle, double speed);
void read_temts();
bool check_wall();
double temt_boundary();

double get_goalie_x();
double get_goalie_y();
void goalie_coords(int bx, int by);

double get_ball_x();
double get_ball_y();

///////////////////////////////////////////////////////////

double read_camera();
double face_goal(double goal_angle);

#endif