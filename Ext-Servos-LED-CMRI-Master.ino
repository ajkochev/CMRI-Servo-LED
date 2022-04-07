//Much of the code here is from Rob's Little Wicket Railway youtube.com channel.  Please watch his videos on CMRI, Servos and Slowing Servos as he goes over much of the code.
//I just combined code from several of his videos and some of my own to reduce servo jitter.

// INCLUDE LIBRARIES
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>
#include <Servo.h>

#define CMRI_ADDR 2 //Set CMRI address, change if needed
#define DE_PIN 2  //Ardino pin 
#define numServos 32 //The number of servos and LEDs connected to all PCA9685 boards.

//SET PCA9685 BOARD ADDRESSES
//For best results run only servos on one board and only LEDs on another.  See setup loop below and setPWMFreq comments.
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x40); //setup the first board address 0
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x41); //setup the second board address 1
//Add more boards here with set addresses.
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);  //Initalize CMRI with Node Address,  Inputs, Outputs and bus speed (19200 baud).  See "bus.begin" in void setup. 
Servo Current_Servo;

//Below sets tables for servos-LEDs.  Be sure enough values are set per numServos above in each table.  16 values for one board, 32 for two boards, etc.
int Status[numServos]; //Create a table to hold the status of each servo-LED
int OldStatus[numServos]; //Create a table to hold the old status of each servo-LED
//Since servos are on Board one, the first 16 numbers are for them, last 16 are for board two all LEDs.  Change values if boards are using LEDs or servos.
int MinValue[numServos] = {800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,800,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
//Above Sets Servo Minimum Values.  First number is for servo on plug 0 on PCA9685 board, second is plug 1 and so on.  For LED use 0 value for off.
int MaxValue[numServos] = {1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,1500,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096,4096};
//Above Sets Servo Maximum Values.  First number is for servo on plug 0 on PCA9685 board, second is plug 1 and so on.  For LED use 4096 for full brightness.
int CurrentPosition[numServos];
int StepSize[numServos] = {5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32,32};
//Above Sets Steps for each Servo and LED
int DelayTime[numServos] = {30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,30,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5};
//Above Sets Delay time for each Servo and LED
unsigned long previousMillis[numServos];

//Servo Move and LED Function
void moveServos(int connection)
{
  unsigned long currentMillis = millis();
  if ((OldStatus[connection] != Status[connection]) && (currentMillis - previousMillis[connection] >= DelayTime[connection]))
  {
    previousMillis[connection] = currentMillis;
    if (Status[connection] == 1)
    {
      if (CurrentPosition[connection] < MaxValue[connection])
      {
        CurrentPosition[connection] = CurrentPosition[connection] + StepSize[connection];
        if (connection <= 16) //Move servos on PCA9685 Board 1
        {
          Current_Servo.attach(connection);
          pwm1.writeMicroseconds(connection, CurrentPosition[connection]);
          Current_Servo.detach();
        }
        if ((connection >= 17) && (connection <= 32)) //Change LEDs on PCA9685 Board 2
        {
          pwm2.writeMicroseconds(connection-16, CurrentPosition[connection]);
        }
        //Add additional if (connection equals) statements here if you add additional boards.  Be sure to use the correct "if" above for LEDs or Servos.
      }
      else
      {
          OldStatus[connection]=Status[connection];
      }
    }
    else
    {
      if (CurrentPosition[connection] > MinValue[connection])
      {
        CurrentPosition[connection] = CurrentPosition[connection] - StepSize[connection];
        if (connection <= 16)  //Move servos on PCA9685 Board 1
        {
          Current_Servo.attach(connection);
          pwm1.writeMicroseconds(connection, CurrentPosition[connection]);
          Current_Servo.detach();
        }
        if ((connection >= 17) && (connection <= 32)) //Change LEDs on PCA9685 Board 2
        {
          pwm2.writeMicroseconds(connection-16, CurrentPosition[connection]);
        }
        //Add additional if (connection equals) statements here if you add additional boards
      }
      else
      {
          OldStatus[connection]=Status[connection];
      }
    }
  }
}


void setup() 
{
  Serial.begin(19200);
  bus.begin(19200);
  pwm1.begin();
  pwm1.setPWMFreq(30);  // Use 30 on boards running just servos to reduce servo jitter.  Using this setting in LEDs will cause flicker.
  pwm2.begin();
  pwm2.setPWMFreq(50);  // This is the maximum PWM frequency.  Use this on boards running just LEDs.  Can cause servo jitter.
  
 for (int i = 0; i < numServos; i=i+1)
  {
     CurrentPosition[i]=MinValue[i]; //Initalize CurrentPosition on Arduino Startup
     OldStatus[i]=1; //Initalize OldStatus on Arduino Startup
  }
}

void loop(){
   cmri.process();

    for (int i = 0; i < numServos; i++) {
        Status[i] = (cmri.get_bit(i));
        if (Status[i] != OldStatus[i])
        {
           moveServos(i); 
        }
    }
}
