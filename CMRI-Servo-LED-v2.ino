/* CMRI Servo LED v2 Sketch By Anthony Kochevar v2-10-2022
 * 
 * This sketch gives you up to 64 ports on four chanined PCA9685 boards.
 * You will need to connect them properly and solder the address jumpers on each
 * correctly and enter those values in the code below.
 * You set the movement values in the deviceVal array.
 * Position1 is first column, end position is Position2 column.
 * Steps defines how fast the servo moves or LED fades on or off. Lower is slower higher is faster.
 * Starting Position sets the positon of the servo or LED on Arduino boot.
 *
 * Warning:  Be sure values used in the deviceVal array are compatable with your servos.
 * Never use LED values with a servo plugged in as it could damage the servo.
 * A good range for servos is 700 to 1400.  LEDs can be 0(off) to 4096(Full Bright)
 * or anywhere in between.
 * If CMRI_ADDR is 3 then the CMRI node in JMRI will be 3 and hardware addresses will start at 3001.
 * 
 * Please read further comments in the code below.
*/

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>

#define CMRI_ADDR 3  //  CMRI node that will be used in JMRI.  Change as needed.
#define DE_PIN    2  // Arduino pin to use internally for CMRI serial data.  Do not change.


Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver (0x40);
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver (0x41);
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver (0x42);
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver (0x43);
Auto485 bus (DE_PIN);
CMRI cmri (CMRI_ADDR, 0, 64, bus);  // CMRI setup with node address, 0 inputs, and 64 outputs.

// ------------------------------------------------
#define numData  4 // Data columns in the array.  Do Not Change.
#define numDevices 64 // Total number of devices availible. 
#define Period  40  // Change to speed up(lower value) or slow down(higher value) movement or fade.  Affects all devices. Value is milliseconds.

int deviceVal[numDevices][numData] = {
//  --Position1--Position2--Steps--Starting Position--
    {    900,       1100,     10,        0 },  // Start on pwm1 port 0 JMRI Address #001
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },  // End on pwm1 port 15 JMRI address #016
    {    900,       1100,     10,        0 },  // Start on pwm2 port 0 JMRI Address #017
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {      0,       4096,     100,       0 },  // This is a example of a LED on port #028.  0=Off 4096=Full On.  
    {    900,       1100,     10,        0 },  // Steps will fade on and off LED.  Higher is faster.
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },  // End on pwm2 port 15 JMRI address #032
    {    900,       1100,     10,        0 },  // Start on pwm3 port 0 JMRI Address #033
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },  // End on pwm3 port 15 JMRI address #048
    {    900,       1100,     10,        0 },  // Start on pwm4 port 0 JMRI Address #049
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },
    {    900,       1100,     10,        0 },  // End on pwm4 port 15 JMRI address #064
};

int devicePos[numDevices];
int deviceTarg[numDevices];
bool lastMove[numDevices];

unsigned long msecLst;

// -----------------------------------------------------------------------------
void loop () {

    cmri.process();
    for (int x = 0; x < numDevices; x++)  {
      if (cmri.get_bit(x) == 1)  {
        deviceTarg[x] = deviceVal[x][1];
      }
      else if (cmri.get_bit(x) == 0) {
        deviceTarg[x] = deviceVal[x][0];
      }
    }

    unsigned long msec = millis();
    if ( (msec - msecLst) >= Period)  {
        msecLst = msec;
        for (int s = 0; s < numDevices; s++)  {
            if (deviceTarg[s] > devicePos[s]) {
              devicePos[s] += deviceVal[s][2];
              if (devicePos[s] >= deviceTarg[s]) { devicePos[s] = deviceTarg[s]; lastMove[s] = true; }
            }
            else if (deviceTarg[s] < devicePos[s]) {
              devicePos[s] -= deviceVal[s][2];
              if (devicePos[s] <= deviceTarg[s]) { devicePos[s] = deviceTarg[s]; lastMove[s] = true; }
            }
            if ((devicePos[s] != deviceTarg[s]) || (lastMove[s] == true)) {  // This if will test for and only move the servo or LED if needed.
              if (s < 16) pwm1.writeMicroseconds (s, devicePos[s]);
              if ((s >= 16) && (s < 32)) pwm2.writeMicroseconds (s - 16, devicePos[s]);
              if ((s >= 32) && (s < 48)) pwm3.writeMicroseconds (s - 32, devicePos[s]);
              if ((s >= 48) && (s < 64)) pwm4.writeMicroseconds (s - 48, devicePos[s]);
              if (devicePos[s] == deviceTarg[s]) lastMove[s] = false;
            }

        }  // End for s
    } //End if msec
}  // End void loop

// -----------------------------------------------------------------------------
void setup () {
    Serial.begin (19200);
    bus.begin (19200);
    pwm1.begin ();
    pwm1.setPWMFreq (50);
    pwm2.begin ();
    pwm2.setPWMFreq (50);
    pwm3.begin ();
    pwm3.setPWMFreq (50);
    pwm4.begin ();
    pwm4.setPWMFreq (50);

    for (int s = 0; s < numDevices; s++) {
      if (deviceVal[s] == 1) {
        devicePos[s] = deviceVal[s][1];
        deviceTarg[s] = devicePos[s];
        lastMove[s] = false;
      }
      else {
        devicePos[s] = deviceVal[s][0];
        deviceTarg[s] = devicePos[s];
        lastMove[s] = false;
      }
      if (s < 16) pwm1.writeMicroseconds (s, devicePos[s]);
      if ((s >= 16) && (s < 32)) pwm2.writeMicroseconds (s - 16, devicePos[s]);
      if ((s >= 32) && (s < 48)) pwm3.writeMicroseconds (s - 32, devicePos[s]);
      if ((s >= 48) && (s < 64)) pwm4.writeMicroseconds (s - 48, devicePos[s]);
    }
} // End void setup

