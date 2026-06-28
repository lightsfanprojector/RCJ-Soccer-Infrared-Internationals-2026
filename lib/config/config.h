#ifndef config_h
#define config_h

#define PIKACHU 1

#define GOALIE 1
#define STRIKER 0

#define ROBOT_DIAMETER 180.0 //mm

#define FIELD_X 1820.0 //mm
#define MID_X 910.0
#define FIELD_Y 2430.0
#define GOAL_LEFT_X 610.0 //left corner of goal for cam localisation
#define GOAL_RIGHT_X 1210.0
#define GOAL_SIZE 600.0 // goal width
#define GOAL_FRONT_Y 2364.0 //goal in front
#define GOAL_BACK_Y 66.0 //goal behind back of the goal (including the thickness of the goal)

#define OUT_FIELD_SIZE 140 // 3005 //mm, out of bounds area
#define OUT_PENALTY_SIZE 390 //mm, out of bounds area + penalty size 25+25+2

#define BCZ_ANGLE_LEFT 355.0 
#define BCZ_ANGLE_RIGHT 29.0


//////////////////////////////new!!!
// #define WHITE_VALUE 
// #define GREEN_VALUE
// #define DIFFERENCEGW 
//////////////////////////////


// #define ORBIT_LB 550
// #define ORBIT_UB FIELD_X - ORBIT_LB

#ifdef PIKACHU
#define pika 1

// // // //GREY
#define BALL_INTENSITY 127 
#define SE_speed 1
#define SE_dir 26
#define NE_speed 27 
#define NE_dir 28
#define SW_speed 18
#define SW_dir 19
#define NW_speed 20
#define NW_dir 21
#define SE_cw -1
#define NE_cw 1
#define NW_cw 1
#define SW_cw -1

//grey
#define US_L_I 15 //input: echo, output: trig
#define US_L_O 14
#define US_B_I 7
#define US_B_O 6
#define US_R_I 16
#define US_R_O 17
#define US_F_I 11
#define US_F_O 10

#define L_m 0.0187
#define L_c -41.4
#define R_m 0.0181
#define R_c -40.7
#define F_m 0.0181
#define F_c -41.9
#define B_m 0.0201
#define B_c -12.9

#define FRONT_LIDAR_MAX 130.0 //calibrate 
#define BACK_LIDAR_MAX 120.0 //calibrate 


////////////////////////////////////////////////////////////

//WHITE 
// #define BALL_INTENSITY 125 //calibrate 
// #define SE_dir 1 
// #define SE_speed 27
// #define NE_speed 26 
// #define NE_dir 28
// #define SW_speed 21
// #define SW_dir 19
// #define NW_speed 20
// #define NW_dir 18
// #define SE_cw -1
// #define NE_cw 1
// #define NW_cw 1
// #define SW_cw 1

// // white
// #define US_L_I 15 //input: echo, output: trig
// #define US_L_O 14
// #define US_B_I 6
// #define US_B_O 7
// #define US_R_I 17
// #define US_R_O 16
// #define US_F_I 10
// #define US_F_O 11

// #define L_m 0.0166
// #define L_c -7.29
// #define R_m 0.0165
// #define R_c -6.34
// #define F_m 0.017
// #define F_c -7.31
// #define B_m 0.0165
// #define B_c -7.56

// #define FRONT_LIDAR_MAX 127.0 //calibrate 
// #define BACK_LIDAR_MAX 120.0 //calibrate 

///////////////////////////////////////////

#define FRONT_LIDAR_SDA 2 
#define FRONT_LIDAR_SCL 3
#define BACK_LIDAR_SDA 8
#define BACK_LIDAR_SCL 9

#define BCZ_PIN 26 
#define BCZ_THRESH 20

#else
#define pika 0

//bot two config
//do later

// #define SE_speed 6
// #define SE_dir 14
// #define NE_speed 7 
// #define NE_dir 15
// #define SW_speed 2
// #define SW_dir 3
// #define NW_speed 4
// #define NW_dir 5
// #define SE_cw 1
// #define NE_cw -1
// #define NW_cw -1
// #define SW_cw -1


// #define SE_dir 26
// #define NE_speed 27 
// #define NE_dir 28
// #define SW_speed 21
// #define SW_dir 19
// #define NW_speed 20
// #define NW_dir 18
// #define SE_cw -1
// #define NE_cw 1
// #define NW_cw 1
// #define SW_cw -1

// #define SE_speed 27
// #define SE_dir 28
// #define NE_speed 26 
// #define NE_dir 1
// #define SW_speed 18
// #define SW_dir 19
// #define NW_speed 20
// #define NW_dir 21
// #define SE_cw -1
// #define NE_cw 1
// #define NW_cw 1
// #define SW_cw -1

#define SE_dir 28 ///correct
#define SE_speed 27
#define NE_speed 26 
#define NE_dir 1
#define SW_speed 21
#define SW_dir 19
#define NW_speed 20
#define NW_dir 18
#define SE_cw -1
#define NE_cw 1
#define NW_cw 1
#define SW_cw -1

#define US_L_I 0 //input: echo, output: trig
#define US_L_O 1
#define US_B_I 22
#define US_B_O 27
#define US_R_I 21
#define US_R_O 20
#define US_F_I 18
#define US_F_O 19

#define FRONT_LIDAR_MAX 110.0
#define BACK_LIDAR_MAX 120.0
#define FRONT_LIDAR_SDA 12 
#define FRONT_LIDAR_SCL 13
#define BACK_LIDAR_SDA 10
#define BACK_LIDAR_SCL 11

#define BCZ_PIN 26
#define BCZ_THRESH 85

#endif

#endif