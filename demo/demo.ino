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
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);
/* Initialize an instance of a servo */
Servo servoOne;  // create servo object to control a servo

/* Global vars go here */
int servoPos = 0;                     // servo position
float lastZ = 0;                      // last Z reading from magnetometer
unsigned long lastReset = 0;          // last reset time
const long resetThreshold = 1000;     // reset threshold

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("HMC5883 Magnetometer"); 
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");  
  Serial.println("Servo Attached.");
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}

void setup(void) 
{
  /* Initialize Serial */
  Serial.begin(9600);  
  /* Initialise the magnetometer */
  if(!mag.begin())
  {   
    Serial.println("Ooops, no HMC5883 detected ... Check your wiring!");
    while(1);
  }    
  /* Attach the servo */ 
  servoOne.attach(9);
  /* Print setup specs */
  displaySensorDetails();
}


void loop(void) 
{
  /* Get a new sensor event */   
  sensors_event_t event; 
  mag.getEvent(&event);

  /* update servo position */
  if(abs(event.magnetic.z - lastZ) > 50 && millis()-lastReset > resetThreshold){
    lastReset = millis();
    servoPos ^= 90;               
  }
  servoOne.write(servoPos);     
 
  /* Display the results (magnetic vector values are in micro-Tesla (uT)) */
  Serial.print("X: "); Serial.print(event.magnetic.x); Serial.print("  ");
  Serial.print("Y: "); Serial.print(event.magnetic.y); Serial.print("  ");
  Serial.print("Z: "); Serial.print(event.magnetic.z); Serial.print("  "); Serial.print("uT ");
  Serial.print("ServoPos: "); Serial.print(servoPos); Serial.println(" ");

  /* save last Z value */
  lastZ = event.magnetic.z;
}
