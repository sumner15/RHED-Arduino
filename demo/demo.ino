/***************************************************************************
  Author: Sumner Norman
  This demonstration code will facilitate the use of the magnetometer as 
  an input device that controls the output to the servos.

  Designed specifically to work with the Adafruit HMC5883 Breakout
  http://www.adafruit.com/products/1746
 
  *** You will also need to install the Adafruit_Sensor library! ***
  These displays use I2C to communicate, 2 pins are required to interface.

 ***************************************************************************/

#include <Servo.h>
#include <Wire.h>
#define address 0x1E  //0011110b, I2C 7bit address of HMC5883
Servo servoOne;       // index finger servo

/* Global vars go here */
int servoPos = 0;                     // servo position
int x,y,z;                            // triple axis data 
float lastZ = 0;                      // last Z reading from magnetometer
const float zThreshold = 200;         // change in Z needed for new position
unsigned long lastReset = 0;          // last reset time
const long resetThreshold = 1000;     // reset threshold

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
  Serial.print(" uT ");
}

void setup(void) 
{ 
  /* Attach the servo */ 
  servoOne.attach(9);
  
  //Initialize Serial and I2C communications
  Serial.begin(9600);
  Wire.begin();
  //Put the HMC5883 IC into the correct operating mode
  Wire.beginTransmission(address);  //open communication with HMC5883
  Wire.write(0x02);                 //select mode register
  Wire.write(0x00);                 //continuous measurement mode
  Wire.endTransmission();
}


void loop(void) 
{
  /* Update thes sensor readings*/   
  updateMag();

  /* update servo position */
  if(abs(z - lastZ) > zThreshold && millis()-lastReset > resetThreshold){  
    lastReset = millis();
    servoPos ^= 60;               
  }
  Serial.print("ServoPos: "); Serial.print(servoPos+60); Serial.println(" ");
  servoOne.write(servoPos+60);         
  delay(250);

  /* save last Z value */
  lastZ = z;
}
