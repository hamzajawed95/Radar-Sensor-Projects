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
int GreenLED=25;//Pin number for onboard Green LED
int RedLED=26;//Pin number for onboard Red LED
int BlueLED=27;//Pin number for onboard Blue LED
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

/* CRGB level bar settings */
const unsigned char levelBarDataPin = 5; // Data pin for CRGB level bar
const unsigned char levelBarSize = 10;   // Pixel size of CRGB level bar

CRGBLevelBar* levelBar;  
BGT60AiPMKR* shield; 

Error_t err;
int no_of_patters;

void GestureHasPerformed();
bool DirHasChanged();


/* Settings for level control */
CtrlInfo ctrlInfo = 
  {.levelPTime = 100.0f, // Set level in/decreasing speed
  .updateRate = 10.0,   // Set level update rate
  .gatingTime = 500,    // Set time before exist gesture is detected after entering cmd mode
  .startTime = 0,       // Auxiliary variable, set during process
  .lastUpdated = 0,     // Auxiliary variable, set during process
  .counter = 0          // Auxiliary variable, set during process
  };        




/* Functions */
bool (*levelControl)(float* outlevel, CtrlInfo* info);
void (*setOutput)(float outlevel);


/* Create radar object with following arguments:
 *  TD : Target Detect Pin
 *  PD : Phase Detect Pin */
Bgt60Ino radarShield(TD, PD);

/* Begin setup function - takes care of initialization and executes only once post reset */
void setup()
{
    /* Set the baud rate for sending messages to the serial monitor */
    Serial.begin(9600);
    levelBar = new CRGBLevelBar(levelBarSize, levelBarDataPin);  // Set up level bar
    levelBar->setBrightness(1);     // Set not higher than 30% when LED supplied by uC!!!
    levelBar->setColorScheme(B2G10Scheme);
    levelControl = &linearControl;
    setOutput = &setPCVolume;
    levelBar->test();                // Let level bar blink to show gesture routine is ready
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

  
  /* Output level to be controlled*/
  float outputLevel = 0.0;

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
          ctrlInfo.startTime = millis();  // Set time reference for control mode

        }
      break;
     

   /* if(servopos==90)
    {
    myservo.write(0);
    servopos=0;
    digitalWrite(Led,HIGH);
    }
    else{
    myservo.write(90);
    servopos=90;
    digitalWrite(Led,LOW);

    }*/

      case COMMAND:

        exitCommand = levelControl(&outputLevel, &ctrlInfo);
        setOutput(outputLevel);
        levelBar->setValue((uint8_t)outputLevel);
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

bool linearControl(float* outlevel, CtrlInfo* info)
{
  
  bool exitFlag = false;

  /* Get exit gesture */
  if(millis() - info->startTime > info->gatingTime)
  {

        if(DirHasChanged()==true)
        {
          exitFlag = true;
        }

    /*

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
                  //curr_state=0;
                  //Serial.println("No Direction");
              break;
          }

      }

        if(prev_state!=curr_state)
        {

          exitFlag = true;
          prev_state=curr_state;

        } */

  }

  /* Adjust output level*/
  if((millis()-info->lastUpdated > 1000.0/info->updateRate) && !exitFlag)
  {

    //Serial.println("yes4");

    err = radarShield.getDirection(direction);
    if (err == OK)
    {

      if(direction==depart)
      {
        /** Increase level **/
        if (*outlevel < 100.0)
        {
          *outlevel += info->levelPTime/info->updateRate;

          if (*outlevel > 100)
          {
            *outlevel = 100; // Ensure value stays in range
          }
      }
      
      }
      else
      {  
        /** Decrease level **/
        if(*outlevel > 5)
        {
        
          *outlevel -= info->levelPTime/info->updateRate;
          if (*outlevel < 5)
          {
            *outlevel = 5; // Ensure value stays in range
          }
        }
      }

    }
    info->lastUpdated = millis();
  }  

  return exitFlag;
}






void initSimpleLED(void)
{
  pinMode(5, OUTPUT);
}





void setSimpleLED(float outlevel)
{
  uint32_t bitLevel = 0;
  bitLevel = round((outlevel/100.0)*255);
  analogWrite(5, bitLevel);
}





void setPCVolume(float outlevel)
{
  Serial.println(outlevel);
}



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