////////////////////////////////////////////////////////////////////////////////
//                                  Authors                                   //
//----------------------------------------------------------------------------//
// Dawei Zhang                                                                //
// Marvin Anthony Mallari                                                     //
// Kai He                                                                     //
////////////////////////////////////////////////////////////////////////////////
//                                   Rules                                    //
//----------------------------------------------------------------------------//
// Provide github or author for any external libraries                        //
// Dynamic Memory (RAM) is small, use as little as possible                   //
//   -If possible use #define, const, byte, PROGMEM                           //
//   -Avoid int, float, long unless absolutely necessary                      //
// Fast code should not have any Serial.print or must be controllable         //
////////////////////////////////////////////////////////////////////////////////
//                                   Notes                                    //
//----------------------------------------------------------------------------//
// MPU6050 I2C address multiplexing schema: [WORKING 20 October 2018]         //
// * The MPU-6050 supports 2 I2C addresses, 0x68 & 0x69                       //
// * The GY-521 breakout board pulls AD0 low to set address to 0x68           //
// * This address manipulation schema works like Chip Select                  //
// * We will use pins defined in CSpin[] to pull AD0 lines LOW or HIGH        //
// ** Initially all CSpin[]s will be set HIGH 0x69                            //
// ** To read an individual sensor, a single CSpin will be set LOW 0x68       //
// ** Tockn's library will read the sensor based on default address 0x68      //
// ** After reading complete, said pin will be set HIGH 0x69                  //
// ** This repeats until all sensors are read.                                //
//                                                                            //
// MPU6050 drift, limitations, work around [WORK IN PROGRESS]                 //
// * USE DATATYPE ANGLE (calibrated with accelerometer)                       //
// * New schema based on vector guessing avoids sensor drift issue            //
//                                                                            //
// Capacitive Touch schema:                                                   //
// * using vector guessing to figure out position instead of trinonometry math//
// ** this method is good since it is less affected by sensor drift           //




#include "MPU6050_tockn_CUSTOM.h"
#include <Wire.h>
#include "Keyboard.h"
#include <CapacitiveSensor.h>

// define i2c address change pins using Arduino pins A0, D15, D14, D16, D10
const byte CSpin[] = {A0,15,14,16,10};

CapacitiveSensor cs_4_5 = CapacitiveSensor(4,5);
CapacitiveSensor cs_4_6 = CapacitiveSensor(4,6);
CapacitiveSensor cs_4_7 = CapacitiveSensor(4,7);
CapacitiveSensor cs_4_8 = CapacitiveSensor(4,8);
CapacitiveSensor cs_4_9 = CapacitiveSensor(4,9);

//define button pins
#define BTN1 A3
#define BTN2 A2
#define BTN3 A1

// xyz-axis for all fingers
float MPUX[] = {0,0,0,0,0};
float MPUY[] = {0,0,0,0,0};
float MPUZ[] = {0,0,0,0,0};

// xyz-axis as initial
float INITMPUX[] = {0,0,0,0,0};
float INITMPUY[] = {0,0,0,0,0};
float INITMPUZ[] = {0,0,0,0,0};

// initialize MPU6050 objects
MPU6050 MPU0(Wire);
MPU6050 MPU1(Wire);
MPU6050 MPU2(Wire);
MPU6050 MPU3(Wire);
MPU6050 MPU4(Wire);

#define KEY_BACKSPACE 178

// define keyboard 
char Keys[] =   {  KEY_BACKSPACE , 'w' , 's', 'a','d'};
boolean currentPressed[] = { false, false, false, false, false, false };

// Capactive touch threashhold, you might want to mess with this if you find its too
// sensitive or not sensitive enough
#define THRESH 6600

long total1 = 0.0;
long total2 = 0.0;
long total3 = 0.0;
long total4 = 0.0;
long total5 = 0.0;


void setup() {
  //delay(3000);
  Wire.begin(); //start I2C
  Serial1.begin(115200); // set baud-rate on ATMEGA 32u4 secondary UART port.
  // serial bluetooth module needs to be configured to 115200 baud with "AT+BAUD8" (115200), default "AT+BAUD4" (9600)
  cs_4_5.set_CS_AutocaL_Millis(0xFFFFFFFF); // turn off autocalibrate on channel 1 - just as an example for total1
  Serial.begin(115200);
  
  for(byte i = 0; i < 5; i++){
    pinMode(CSpin[i], OUTPUT);
    digitalWrite(CSpin[i], HIGH); // set all MPU6050 addresses to 0x69 by pulling AD0 HIGH, default 0x68 pulled LOW
  }

  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  
  //i2cScan();
  
  digitalWrite(CSpin[0],LOW);
  delay(10);
  MPU0.begin();
  MPU0.calcGyroOffsets(true);
  digitalWrite(CSpin[0],HIGH);
  digitalWrite(CSpin[1],LOW);
  delay(10);
  MPU1.begin();
  MPU1.calcGyroOffsets(true);
  digitalWrite(CSpin[1],HIGH);
  digitalWrite(CSpin[2],LOW);
  delay(10);
  MPU2.begin();
  MPU2.calcGyroOffsets(true);
  digitalWrite(CSpin[2],HIGH);
  digitalWrite(CSpin[3],LOW);
  delay(10);
  MPU3.begin();
  MPU3.calcGyroOffsets(true);
  digitalWrite(CSpin[3],HIGH);
  digitalWrite(CSpin[4],LOW);
  delay(10);
  MPU4.begin();
  MPU4.calcGyroOffsets(true);
  digitalWrite(CSpin[4],HIGH);
}

boolean reduceMode = false;
byte rMtype = 0;
byte sType = 0;

void loop() {
  updateAll();
  //long start = millis();
  total1 =  cs_4_5.capacitiveSensor(30); // 30 SAMPLES
  total2 =  cs_4_6.capacitiveSensor(30);
  total3 =  cs_4_7.capacitiveSensor(30);
  total4 =  cs_4_8.capacitiveSensor(30);
  total5 =  cs_4_9.capacitiveSensor(30);
  
  if(digitalRead(BTN1) == LOW){ //turn on verbose mode
    delay(250);
    reduceMode = !reduceMode; //toggle
    Serial1.print("DEV MODE: ");
    Serial1.println(reduceMode);
  }
  if(digitalRead(BTN2) == LOW){ //change sensor type
    delay(250);
    sType++;
    if(sType > 4){
      sType = 0;
    }
  }
  if(digitalRead(BTN3) == LOW){ //change axis
    delay(250);
    rMtype++;
    if(rMtype > 3){
      rMtype = 0;
    }
  }
  if(reduceMode){
    if(sType == 0){
      Serial1.print("DATA TYPE: GYRO: ");
      updateGyro();
    }
    if(sType == 1){
      Serial1.print("DATA TYPE: GYRO ANGLE: ");
      updateGyroAngle();
    }
    if(sType == 2){
      Serial1.print("DATA TYPE: ANGLE: ");
      updateAngle();
    }
    if(sType == 3){
      Serial1.print("DATA TYPE: ACCELEROMETER: ");
      updateAccel();
    }
    if(sType == 4){
      Serial1.print("DATA TYPE: TOUCH: ");
      Serial1.print(total1);                  // print sensor output 1
      Serial1.print("\t");
      Serial1.print(total2);                  // print sensor output 2
      Serial1.print("\t");
      Serial1.print(total3);                // print sensor output 3
      Serial1.print("\t");
      Serial1.print(total4);                // print sensor output 3
      Serial1.print("\t");
      Serial1.println(total5);                // print sensor output 3
    }
    
    if(rMtype == 0 && sType != 4) {
      printX();
      printY();
      printZ();
      Serial1.println(); 
    }
    
    else if(rMtype == 1 && sType != 4) {
      printX();
      Serial1.println();
    }
    
    else if(rMtype == 2 && sType != 4) {
      printY();
      Serial1.println();
    }
    
    else if(rMtype == 3 && sType != 4) {
      printZ();
      Serial1.println();
    }
  }
  else{
    keyboardEngine();
  }

  //RUN SHORT RECALIBRATION
  if(digitalRead(BTN1) == LOW && digitalRead(BTN3) == LOW){
    digitalWrite(CSpin[0],LOW);
    delay(10);
    MPU0.quickCalcOffset(true);
    digitalWrite(CSpin[0],HIGH);
    digitalWrite(CSpin[1],LOW);
    delay(10);
    MPU1.quickCalcOffset(true);
    digitalWrite(CSpin[1],HIGH);
    digitalWrite(CSpin[2],LOW);
    delay(10);
    MPU2.quickCalcOffset(true);
    digitalWrite(CSpin[2],HIGH);
    digitalWrite(CSpin[3],LOW);
    delay(10);
    MPU3.quickCalcOffset(true);
    digitalWrite(CSpin[3],HIGH);
    digitalWrite(CSpin[4],LOW);
    delay(10);
    MPU4.quickCalcOffset(true);
    digitalWrite(CSpin[4],HIGH);
  }
}

void printX(){
  Serial1.print("X: ");
  for(byte i = 0; i < 5; i++){
    Serial1.print(MPUX[i]);
    Serial1.print(", ");
  }
}

void printY(){
  Serial1.print("Y: ");
  for(byte i = 0; i < 5; i++){
    Serial1.print(MPUY[i]);
    Serial1.print(", ");
  }
}

void printZ(){
  Serial1.print("Z: ");
  for(byte i = 0; i < 5; i++){
    Serial1.print(MPUZ[i]);
    Serial1.print(", ");
  }
}

// READ ALL SENSORS AND STORE THEM IN THEIR OWN OBJECT MEMORY
void updateAll(){
  digitalWrite(CSpin[0],LOW);
  delay(10);
  MPU0.update();
  digitalWrite(CSpin[0],HIGH);
  delay(10);
  digitalWrite(CSpin[1],LOW);
  delay(10);
  MPU1.update();
  digitalWrite(CSpin[1],HIGH);
  delay(10);
  digitalWrite(CSpin[2],LOW);
  delay(10);
  MPU2.update();
  digitalWrite(CSpin[2],HIGH);
  delay(10);
  digitalWrite(CSpin[3],LOW);
  delay(10);
  MPU3.update();
  digitalWrite(CSpin[3],HIGH);
  delay(10);
  digitalWrite(CSpin[4],LOW);
  delay(10);
  MPU4.update();
  digitalWrite(CSpin[4],HIGH);
}

void updateGyro(){
  MPUX[0] = MPU0.getGyroX();
  MPUX[1] = MPU1.getGyroX();
  MPUX[2] = MPU2.getGyroX();
  MPUX[3] = MPU3.getGyroX();
  MPUX[4] = MPU4.getGyroX();
  
  MPUY[0] = MPU0.getGyroY();
  MPUY[1] = MPU1.getGyroY();
  MPUY[2] = MPU2.getGyroY();
  MPUY[3] = MPU3.getGyroY();
  MPUY[4] = MPU4.getGyroY();
  
  MPUZ[0] = MPU0.getGyroZ();
  MPUZ[1] = MPU1.getGyroZ();
  MPUZ[2] = MPU2.getGyroZ();
  MPUZ[3] = MPU3.getGyroZ();
  MPUZ[4] = MPU4.getGyroZ();
}

void updateGyroAngle(){
  MPUX[0] = MPU0.getGyroAngleX();
  MPUX[1] = MPU1.getGyroAngleX();
  MPUX[2] = MPU2.getGyroAngleX();
  MPUX[3] = MPU3.getGyroAngleX();
  MPUX[4] = MPU4.getGyroAngleX();
  
  MPUY[0] = MPU0.getGyroAngleY();
  MPUY[1] = MPU1.getGyroAngleY();
  MPUY[2] = MPU2.getGyroAngleY();
  MPUY[3] = MPU3.getGyroAngleY();
  MPUY[4] = MPU4.getGyroAngleY();
  
  MPUZ[0] = MPU0.getGyroAngleZ();
  MPUZ[1] = MPU1.getGyroAngleZ();
  MPUZ[2] = MPU2.getGyroAngleZ();
  MPUZ[3] = MPU3.getGyroAngleZ();
  MPUZ[4] = MPU4.getGyroAngleZ();
}

void updateAngle(){
  MPUX[0] = MPU0.getAngleX();
  MPUX[1] = MPU1.getAngleX();
  MPUX[2] = MPU2.getAngleX();
  MPUX[3] = MPU3.getAngleX();
  MPUX[4] = MPU4.getAngleX();
  
  MPUY[0] = MPU0.getAngleY();
  MPUY[1] = MPU1.getAngleY();
  MPUY[2] = MPU2.getAngleY();
  MPUY[3] = MPU3.getAngleY();
  MPUY[4] = MPU4.getAngleY();
  
  MPUZ[0] = MPU0.getAngleZ();
  MPUZ[1] = MPU1.getAngleZ();
  MPUZ[2] = MPU2.getAngleZ();
  MPUZ[3] = MPU3.getAngleZ();
  MPUZ[4] = MPU4.getAngleZ();
}

void updateAccel(){
  MPUX[0] = MPU0.getAccX();
  MPUX[1] = MPU1.getAccX();
  MPUX[2] = MPU2.getAccX();
  MPUX[3] = MPU3.getAccX();
  MPUX[4] = MPU4.getAccX();
  
  MPUY[0] = MPU0.getAccY();
  MPUY[1] = MPU1.getAccY();
  MPUY[2] = MPU2.getAccY();
  MPUY[3] = MPU3.getAccY();
  MPUY[4] = MPU4.getAccY();
  
  MPUZ[0] = MPU0.getAccZ();
  MPUZ[1] = MPU1.getAccZ();
  MPUZ[2] = MPU2.getAccZ();
  MPUZ[3] = MPU3.getAccZ();
  MPUZ[4] = MPU4.getAccZ();
}

//math to do vector guessing
double distanceBetweenTwoPoints(double x, double y, double a, double b){
  return sqrt(pow(x - a, 2) + pow(y - b, 2));
}

void keyboardEngine(){
  //get the orgin
      updateAngle();
       Serial.print("x: ");
       Serial.println(MPUX[0]);
       Serial.print("y: "); 
       Serial.println(MPUY[0]);
       Serial.println(distanceBetweenTwoPoints(MPUX[0],MPUY[0],INITMPUX[0],INITMPUY[0]));
      int keyIndex = 0;
      for (int i = 0; i < 5; i++){ 
        INITMPUX[i] = MPUX[i];
        INITMPUY[i] = MPUY[i];
        INITMPUZ[i] = MPUZ[i];
      }
  // interprets all the data and types the appropriate key
  // check if sensing that a finger is touching the pad
  // and that it wasnt already pressed
   
    if ((total1 > THRESH) && (! currentPressed[0])) { 
      updateAngle();
      if(distanceBetweenTwoPoints(MPUX[0],MPUY[0],INITMPUX[0],INITMPUY[0] )< 10 ){ // Touching is on the middle circle
        keyIndex =0;
        Serial.print("Key pressed #"); 
        Serial.print(keyIndex);
        Serial.print(" ("); 
        Serial.print(Keys[keyIndex]); 
        Serial.println(")");
        currentPressed[keyIndex] = true;
        Keyboard.press( Keys[keyIndex]);
      }
      else{
          if( abs(INITMPUX[0] - MPUX[0]) > abs( INITMPUY[0] - MPUY[0])){ // Detecting the intention of the person's movement
            if( MPUX[0]>0){  // UPPER'w' or LOWER's'
                keyIndex =1;
                Keyboard.press( Keys[1]);
              }
             else{    
                keyIndex =2;
                Keyboard.press( Keys[2]);
              }
          }
          else{
            if( MPUY[0]>0){ // LEFT 'a' or RIGHT 'd'
              keyIndex =3;
              Keyboard.press( Keys[3]);
            }
            else{
              keyIndex =4;
              Keyboard.press( Keys[4]);
            }
          }
      }
    }
    else if ((total1 <= THRESH) && (currentPressed[0])) {
      // key was released (no touch, and it was pressed before)
      Serial.print("Key released #"); 
      Serial.print(0);
      Serial.print(" (");
      Serial.print(Keys[keyIndex]);
      Serial.println(")");
      for (int i = 0; i < 5; i++){ 
        currentPressed[i] = false;
        }
      Keyboard.releaseAll();
    } 
}
