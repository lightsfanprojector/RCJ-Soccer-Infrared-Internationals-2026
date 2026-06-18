#include <Arduino.h>

double temt_final_angle;
double temt_final_speed;

//pio run -t upload -e pico --upload-port COM17     
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

void temt_boundary(double speed, double angle){
    double boundary_angle;

    if ((temt_data[0] == 1) && (temt_data[1] == 1 || temt_data[2] == 1)){
        boundary_angle = 315.0;
        temt_final_angle = 135.0;
    }

    else if ((temt_data[3] == 0 || temt_data[4] == 0) && (temt_data[1] == 1|| temt_data[2] == 1)){
        boundary_angle = 225.0; 
        temt_final_angle = 45.0; 
    }
    
    else if ((temt_data[3] == 0 || temt_data[4] == 0) && (temt_data[5] == 1|| temt_data[6] == 1)){
        boundary_angle = 135.0; 
        temt_final_angle = 315.0; 
    }

    
    else if ((temt_data[5] == 0 || temt_data[6] == 0) && (temt_data[0] == 1)){
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
}




void setup() {
  // put your setup code here, to run once:
    Serial2.setTX(4);
    // int8_t rx = 5;
    // int8_t tx = 4;
    
    Serial2.setRX(5);
    Serial2.begin(9600);
    // Serial2.begin(115200, SERIAL_8N1, (int8_t)5, (int8_t)4); //rx, tx, to esp
    Serial.begin(9600);
}

void loop() {
    // Serial.println(Serial2.available());
    
    read_temts();
    
    for (int i = 0; i < 8; i++){
        Serial.print(temt_data[i]);
        Serial.print(" "); //first 7 outer ring of temts, 8th is bcz temt 
    } 
    Serial.println(" ");

    check_wall();

    // Serial.println(bottom_data);

}


