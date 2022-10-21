CMRI Servo LED v2 Sketch By Anthony Kochevar v2-10-2022

This sketch gives you up to 64 ports on four chanined PCA9685 boards.
You will need to connect them properly and solder the address jumpers on each
correctly and enter those values in the code below.
You set the movement values in the deviceVal array.
Position1 is first column, end position is Position2 column.
Steps defines how fast the servo moves or LED fades on or off. Lower is slower higher is faster.
Starting Position sets the positon of the servo or LED on Arduino boot.
 *
Warning:  Be sure values used in the deviceVal array are compatable with your servos.
Never use LED values with a servo plugged in as it could damage the servo.
A good range for servos is 700 to 1400.  LEDs can be 0(off) to 4096(Full Bright)
or anywhere in between.
If CMRI_ADDR is 3 then the CMRI node in JMRI will be 3 and hardware addresses will start at 3001.
