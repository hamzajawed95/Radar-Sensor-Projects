/*!
 * \name        Smart Switch Demo
 * \author      Muhammad Hamza Jawaid
 * \brief       
 * \details 
 * @date 22-07-2021   
 *
 *            
 */

/******Includes******/
#include <Arduino.h>
#include <Servo.h>
#include <bgt60-ino.hpp>
#include <bgt60-platf-ino.hpp>
#include <WiFiNINA.h>//wifi library for mkrwifi 1010
#include <utility/wifi_drv.h>//library for onboard LED control
#include "..\lib\CRGBLevelBar\src\CRGBLevelBar.h"
#include "..\lib\BGT60AiPMKR\src\BGT60AiPMKR.h"
#include "main.h"



/*
* In case no supported platform is defined, the
* PD and TD pin are set to the values below.
*/
#ifndef TD
#define TD  15
#endif

#ifndef PD
#define PD  16
#endif

/******Defines******/
#define approach Bgt60::APPROACHING
#define depart Bgt60::DEPARTING
#define nodir Bgt60::NO_DIR

/******Variables******/
Servo myservo;
int Led=14;
unsigned long myTime1;
unsigned long myTime2;
unsigned long t1;
unsigned long t2;
const long interval = 2000; 
const long interval_2 = 500; 
int servopos=90;
int prev_state=0;
int curr_state;
Bgt60::Motion_t motion;
Bgt60::Direction_t direction;



Error_t err;
int no_of_patters;

void GestureHasPerformed();
bool DirHasChanged();
bool exitProcess();




/* Create radar object with following arguments:
 *  TD : Target Detect Pin
 *  PD : Phase Detect Pin */
Bgt60Ino radarShield(TD, PD);

/* Begin setup function - takes care of initialization and executes only once post reset */
void setup()
{
    /* Set the baud rate for sending messages to the serial monitor */
    Serial.begin(9600);
    initRGBLed(); 
    pinMode(Led,OUTPUT);
    myservo.attach(9); 
    
    // Configures the GPIO pins to input mode
    Error_t init_status = radarShield.init();
    /* Check if the initialization was successful */
    if (OK != init_status) {
        Serial.println("Init failed.");
    }
    else {
        Serial.println("Init successful.");
    }
}


/* Begin loop function - this part of code is executed continuously until external termination */
void loop()
{
  /* FSM state buffers */
  CtrlState ctrlState = STANDBY;

  /* Auxiliary variables */
  bool exitCommand = false;

  direction = nodir;
  curr_state=0;


while(1)

{
   switch(ctrlState)
    {

      case STANDBY:

        motion = Bgt60::NO_MOTION;
        err = radarShield.getMotion(motion);
        if(err == OK)
        {
          if (motion==Bgt60::MOTION)
          { 
            ctrlState = GESTURE;
            setRGBLed(RED, 20);
          }

          if (motion==Bgt60::NO_MOTION)
          { 
            /** Reset on all states and signals on timeout **/
            ctrlState = STANDBY;
            setRGBLed(NONE, 0);
          }
        }

        else{
          Serial.println("No Motion");
        }

      break;


      case GESTURE:

        GestureHasPerformed();

        if(myTime2-myTime1<interval){
          ctrlState = COMMAND;
          setRGBLed(BLUE, 20);
          delay(1000);                // Give user time to get into position
          setRGBLed(GREEN, 20);
          //ctrlInfo.startTime = millis();  // Set time reference for control mode
          if(servopos==90)
          {
            myservo.write(0);
            servopos=0;
            digitalWrite(Led,HIGH);
          }
          else{
            myservo.write(90);
            servopos=90;
            digitalWrite(Led,LOW);

          }

        }
      break;

      case COMMAND:

        exitCommand = exitProcess();
        if (exitCommand)
        {
        exitCommand = false;
        ctrlState = STANDBY;
        setRGBLed(NONE, 0);
        delay(1000);
        } 
      break;

    } //switch end       
    
} //while(1) end
   
} // loop end




/*******************************************************************/
/*                     MY FUNCTION DEFINITIONS                     */
/*******************************************************************/


void initRGBLed(void)
{
  WiFiDrv::pinMode(26, OUTPUT);  //RED
  WiFiDrv::pinMode(25, OUTPUT);  //GREEN
  WiFiDrv::pinMode(27, OUTPUT);  //BLUE
}



void setRGBLed(RgbColor color, unsigned char level)
{
  uint8_t rComp = 0, gComp = 0, bComp = 0;
  uint8_t bitLevel = 0;
  // Convert level in 10 bit value
  bitLevel = round((level/100.0)*255);

  switch(color)
  {
    case NONE:
        rComp = 0;
        gComp = 0;
        bComp = 0;
      break;

    case RED:
        rComp = bitLevel;
        gComp = 0;
        bComp = 0;
      break;

    case GREEN:
        rComp = 0;
        gComp = bitLevel;
        bComp = 0;
      break;

    case BLUE:
        rComp = 0;
        gComp = 0;
        bComp = bitLevel;
      break;

    case WHITE:
        rComp = bitLevel;
        gComp = bitLevel;
        bComp = bitLevel;
      break;
  }
  WiFiDrv::analogWrite(26, rComp); // Set red component
  WiFiDrv::analogWrite(25, gComp); // Set green component
  WiFiDrv::analogWrite(27, bComp); // Set blue component
}




void GestureHasPerformed(void)
{

      start:               
      myTime1=millis();

      no_of_patters=0; 

      while(no_of_patters<4)
      {

        if(DirHasChanged()==true)
        {
            no_of_patters+=1;
        }

        t2=millis();

        if(t2-myTime1>interval_2)
        {
          goto start;
        }

    }

      myTime2=millis();
}



bool DirHasChanged(void)
{
      bool dirFlag = false;
      err = radarShield.getDirection(direction);
      if (err == OK)
      {
          switch (direction)
          {
              case Bgt60::APPROACHING:
                  curr_state=1;
              break;
              case Bgt60::DEPARTING:
                  curr_state=2;
              break;

              case Bgt60::NO_DIR:
                  curr_state=0;
                  //Serial.println("No Direction");
              break;
          }

      }

        if(prev_state!=curr_state)
        {
          prev_state=curr_state;
          dirFlag = true;
        }

        return dirFlag;

}



bool exitProcess(void)
{
  
  bool exitFlag = false;

  err = radarShield.getMotion(motion);
  if(err == OK)
  {
    if (motion==Bgt60::NO_MOTION) 
      { 
        exitFlag = true;
      }
  }

  return exitFlag;

}