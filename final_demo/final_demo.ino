/***************************************************************************
  Author: Sumner Norman
  This demonstration code will facilitate the use of the magnetometer as 
  an input device that controls the output to the servos for the Robotic
  Hand Extension Device (RHED). 
 ***************************************************************************/

#include <Servo.h>
#include <Wire.h>
#define address 0x1E  //0011110b, I2C 7bit address of HMC5883
Servo servoOne;       // thumb servo
Servo servoTwo;       // index finger servo

/* Global vars go here */
int state = 0;                        // state machine variable
int thumbPos = 0;                     // servo position
int indexPos = 0;                     // servo position
int x,y,z;                            // triple axis data 
float lastZ = 0;                      // last Z reading from magnetometer
unsigned long lastReset = 0;          // last reset time
const float zThreshold = 200;         // change in Z needed for new position
const long resetThreshold = 1500;     // reset threshold
const int flex = 60;                  // flex servo position
const int extend = 120;               // extend servo position

int demoState = 1;                    // for demo purposes only

/******************** update magnetometer function ************************/
void updateMag(void){
  //Tell the HMC5883L where to begin reading data
  Wire.beginTransmission(address);
  Wire.write(0x03); //select register 3, X MSB register
  Wire.endTransmission();
 //Read data from each axis, 2 registers per axis
  Wire.requestFrom(address, 6);
  if(6<=Wire.available()){
    x = Wire.read()<<8;  //X msb
    x |= Wire.read();    //X lsb
    z = Wire.read()<<8;  //Z msb
    z |= Wire.read();    //Z lsb
    y = Wire.read()<<8;  //Y msb
    y |= Wire.read();    //Y lsb
  }
  //Print out values of each axis
  Serial.print("X: ");   Serial.print(x);
  Serial.print("  Y: "); Serial.print(y);
  Serial.print("  Z: "); Serial.print(z);  
  Serial.print(" uT          ");
}

/******************** find maximum value of 1x3 array **********************/
int maxInd(int *dataIn){
  int  index = 0;
  for (int i=1; i<=2; i++){
    if (dataIn[i] > dataIn[i-1]){
      index = i;
    }
  }
  Serial.print("MAXIMUM INDEX IS   "); Serial.print(index);

  /* demo state machine here */
  if (demoState == 3){
    demoState = 1;
  } else {
    demoState = demoState+1;
  }
  index = demoState;
  /* end demo state machine */
  
  return index;
}

/******************************* setup *************************************/
void setup(void) 
{ 
  /* Attach the servo */ 
  servoOne.attach(9);
  servoTwo.attach(10);
  
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address);  //open communication with HMC5883
  Wire.write(0x02);                 //select mode register
  Wire.write(0x00);                 //continuous measurement mode
  Wire.endTransmission();

  Serial.println(" hello world ");
}

/************************** main loop **************************************/
void loop(void) 
{
  /* print current state (for debugging purposes) */
  Serial.print("CURRENT STATE: "); Serial.print(state); Serial.print("  ::::  ");
  
  /* Update the sensor readings*/   
  updateMag(); 

  /* state machine */  
  switch (state) {
    case 0: // both flexed      
      thumbPos = flex;
      indexPos = flex;
      // if the ring is in proximity and we have passed the reset time...
      if(abs(z - lastZ) > zThreshold && millis()-lastReset > resetThreshold){  
        lastReset = millis();     // reset timer reset
        int magVals[3] = {x, y, z};
        state = maxInd(magVals); // determine next state based on max reading
        Serial.print(", NEW STATE IS "); Serial.println(state);
      }
      break;
    case 1: // thumb flex, index extend      
      thumbPos = flex;
      indexPos = extend;
      if (millis()-lastReset > resetThreshold){
        lastReset = millis();     // reset timer reset
        state = 0;        
      }
      break;
    case 2: // index extend, thumb flex      
      thumbPos = extend;
      indexPos = flex;
      if (millis()-lastReset > resetThreshold){
        lastReset = millis();     // reset timer reset
        state = 0;
      }
      break;
    case 3: // both extended      
      thumbPos = extend;
      indexPos = extend;
      if (millis()-lastReset > resetThreshold){
        lastReset = millis();     // reset timer reset
        state = 0;
      }
      break;
    default :
      state = 0;
    break;
  }
  
  /* update servo position */
  Serial.print("thumbPos: "); Serial.print(thumbPos); Serial.print("       ");
  Serial.print("indexPos: "); Serial.print(indexPos); Serial.println(" ");
  // swap thumb position values (servo is physically reversed in real life)
  if (thumbPos == flex){
    thumbPos = extend;
  }else {
    thumbPos = flex;
  }  
  servoOne.write(thumbPos);       
  servoTwo.write(indexPos);  
  delay(15);

  /* save last Z value */
  lastZ = z;
}
