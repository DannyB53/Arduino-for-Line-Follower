/*
This is the sketch for Arduino conversion to an analog Line Follower
The sketch assumes you have an Attiny85 chip. 
There is no reason you could not use other Arduinos like Pro mini, Nano or Uno, but you may need to change the pin assignment (see below).

Code is provided under the GNU public license

The code works fine with Attiny85 configured at 1MHz, but there is no reason it would not work at other frequencies.

Anniny85 needs to be installed in Arduino boards manager of the Arduino IDE before you can download the script to the Attiny85 chip, and you would need some kind of ISP (In System Programmer) like USBTiny or Arduino configured as ISP. 
Attiny85 can be added to Arduino by:
 - Add boards manager URL: https://raw.githubusercontent.com/damellis/attiny/ide-1.6.x-boards-manager/package_damellis_attiny_index.json
   to 'File'- 'Preferences'- 'Additional Boards URL'
   then select 'Tools' - 'Board'- 'Boards Manager'
   seach for Attiny and install. Attiny will now show up in your boards menu.
   Select Attiny85, 1MHz internal clock, and select you programmer (such as USBTiny)
   Before downloading your sketch, burn bootloader   

Have fun with it.
 
 */

// First, let's define on which Attiny85 pins the sensors and motors are.
// When you use a different Arduino, like Pro Mini, Nano or Uno, you would likely need to change these to select the pins you are using.
const int sensorLeft = A3;    
const int sensorRight = A2;  
const int motorLeft = 0;      
const int motorRight = 1;

// Declare and initialize variables

// These variables contain the reading from the light sensitive sensors
int sensorValueLeft = 0;         // the sensor value
int sensorValueRight = 0;

// These variables contain the calibration values (see below)
int sensorThresholdLeft = 0;        // minimum sensor value
int sensorThresholdRight = 0;           // maximum sensor value

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// This is an important parameter you may wish to experiment with. This resembles the offset between light (white) and dark (black) areas.
// -75 works well for us, but you may wish to experiment with this value; vary between -25 and -100 if you like.
// To small (negative) values will cause false black readings (stuttering above white areas) and too large (negative) values will mean the black line is not detected.
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
int SensorOffset = -75;


void setup() {
// This setup routine is executed once during start-up of the Arduino

// initialize Attiny pins as a digital output for driving the motors.
pinMode(motorLeft, OUTPUT);
pinMode(motorRight, OUTPUT);

// Please note that as the motors are driven with PNP resistors, HIGH means the motors stop and LOW means the motors run.
// Initialising the motors to stay quiet.
digitalWrite(0, HIGH);
digitalWrite(1, HIGH);

// Wait for 2 seconds between powerup and calibration. This allows you to place the line follower above a white or light area.
delay(2000);

// Vibrate motors briefly, to indicate the sensors are being calibrated
VibMotors();

// Calibrate sensors (place Line Follower above a white area)
CalSensors();

// Vibrate motors again to indicate calibration is done
VibMotors();

// Wait for another second
delay(1000);

}



void loop() {

// The loop function runs forever

// Read sensors
  sensorValueLeft = analogRead(sensorLeft);
  sensorValueRight = analogRead(sensorRight);

// This is where the voting takes place. There is only simple boolean logic involved.
// Remember that HIGH means motors stop, and LOW means that motors run.

//Left = black and right is black - both sensors are above a black line, so keep running.
 if (sensorValueLeft > sensorThresholdLeft) {
   if (sensorValueRight > sensorThresholdRight) {
     digitalWrite(motorLeft, LOW); 
     digitalWrite(motorRight, LOW); 
    }
 }     

//Left = white and right is white - both sensors are above a white area, so keep running.
 if (sensorValueLeft < sensorThresholdLeft) {
   if (sensorValueRight < sensorThresholdRight) {
     digitalWrite(motorLeft, LOW); 
     digitalWrite(motorRight, LOW); 
    }
 }  

//Left = black and right is white - left sensor is seeing the line, right not, so stop the left motor.
 if (sensorValueLeft > sensorThresholdLeft) {
   if (sensorValueRight < sensorThresholdRight) {
     digitalWrite(motorLeft, HIGH); // Stop motor
     digitalWrite(motorRight, LOW); 
    }
 } 

//Left = white and right is black - right sensor is seeing the line, left not so stop the right motor.
 if (sensorValueLeft < sensorThresholdLeft) {
   if (sensorValueRight > sensorThresholdRight) {
     digitalWrite(motorLeft, LOW); 
     digitalWrite(motorRight, HIGH); //Stop motor
    }
 }    
   

}

void VibMotors()
{
// Vibrate motors by pulsing them with 20 msec intervals.
// Your Line Follower is not supposed to start driving during this interval, so shorten the intervals if this is the case.

  for (int i=0; i <= 5; i++){
    digitalWrite(0, LOW);
    digitalWrite(1, LOW);
     delay(10);    
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);
     delay(20);    
    }
}

void CalSensors()
{

// This is where sensor calibration takes place. 
// The Line Follower is supposed to be placed above a white or light area.
// It is in fact very simple; within 2 seconds, 10 readings are taken, which are averaged.

// Make sure calibration values are empty
sensorThresholdLeft = 0;        // minimum sensor value
sensorThresholdRight = 0;       // maximum sensor value
  
for (int i=0; i < 10; i++){

    // Read sensors
   sensorValueLeft = analogRead(sensorLeft);
   sensorValueRight = analogRead(sensorRight);

  // Simply add the read values to the calibration values 10 times
  sensorThresholdLeft = sensorThresholdLeft + sensorValueLeft;        
  sensorThresholdRight = sensorThresholdRight + sensorValueRight;     
  
     delay(200);    
  }  

 // Calculate average reading by dividing by 10 
 sensorThresholdLeft = sensorThresholdLeft / 10 ;
 sensorThresholdRight = sensorThresholdRight / 10;

 // Add the offset to the calibration value, to make sure that a significant bightness change is needed, before black is detected. 
 sensorThresholdLeft = sensorThresholdLeft + SensorOffset ;
 sensorThresholdRight = sensorThresholdRight +  SensorOffset;  
  
}

// End of code

