#include "SoftwareSerial.h" //library for serial communication 
#include "DFRobotDFPlayerMini.h"//library for the DFplayer mini


#include <Arduino.h>
/* Include library main header */
#include <bgt60-ino.hpp>
/* Include Arduino platform header */
#include <bgt60-platf-ino.hpp>

/*
* In case no supported platform is defined, the
* PD and TD pin are set to the values below.
*/
#ifndef TD
#define TD  5
#endif

#ifndef PD
#define PD  6
#endif

Bgt60Ino radarShield(TD, PD);

SoftwareSerial mySoftwareSerial(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;//creating an object for the library
//const int pdpin = A4;//direction pin on shield
//const int tdpin = A5; //target pin on shield
int state_curr =0;
int state_prev =1;
int state_s1=0;



void setup()
{
  initIO(); // function that declares the setup
   
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


void loop(){

  SM_det();//state machine function declaration

  
}


void initIO(){
  
  mySoftwareSerial.begin(9600);
  Serial.begin(9600);
  Serial.println();
  Serial.println(F("DFRobot DFPlayer Mini"));
  Serial.println(F("Initializing DFPlayer module ... Wait!"));
  if (!myDFPlayer.begin(mySoftwareSerial))
  {
    while (true);
  }
  Serial.println();
  Serial.println(F("DFPlayer Mini module initialized!"));
  myDFPlayer.setTimeOut(500); //Timeout serial 500ms
  myDFPlayer.volume(20);//volume 10
  myDFPlayer.EQ(0); //Equalizer normal
  
}



void SM_det()// state machine function
{

    unsigned long myTime1;
    unsigned long myTime2;

    const long interval = 2000; 
    
    Bgt60::Motion_t motion = Bgt60::NO_MOTION;
    Bgt60::Direction_t direction = Bgt60::NO_DIR;

    Error_t err = radarShield.getMotion(motion);

    /* Check if API execution is successful */
    if (err == OK)
    {

    switch(state_s1)
    {
      case 0:
      if(Bgt60::MOTION == motion){

      err = radarShield.getDirection(direction);

      if(OK == err)
      {
        switch(direction){
          case Bgt60::APPROACHING:
          Serial.println("Approaching");
          myTime1 = millis();
          Serial.println(myTime1);

          while(direction==Bgt60::APPROACHING)
          {
               Error_t err2 = radarShield.getDirection(direction);
               if(OK == err2)
               {
                Serial.println("hj");
            
                myTime2 = millis();
                       
                }
                        
               if((myTime2-myTime1)>interval)
               {
                break;
               }           
            }
            
        Serial.println(myTime2);
        Serial.println(myTime2-myTime1);
        
        if((myTime2-myTime1)>interval)
        {
          Serial.println("Chal beta print");
          state_curr=1;
          if(state_prev!=state_curr){
          state_s1=1;
          Serial.println("\n states1=1 detected");
          }      
        }
        break;


      
        case Bgt60::DEPARTING:

        myTime1 = millis();
        Serial.println(myTime1);

        while(direction==Bgt60::DEPARTING)
        {
          Error_t err2 = radarShield.getDirection(direction);
          if(OK == err2)
          {
            Serial.println("hj2");
            
            myTime2 = millis();
                       
           }
                      
          if((myTime2-myTime1)>interval)
          {
            break;
               
           }
            
          }
  
        Serial.println(myTime2);
        Serial.println(myTime2-myTime1);
        if((myTime2-myTime1)>=interval)
        {
          state_curr=0;
          if(state_prev!=state_curr)
          {
          state_s1=2;
          }
         }
        
        break;
        }
        }
    }
    break;

    
    case 1:
    // approach
    myDFPlayer.playMp3Folder(1);// play the first mp3 file
    delay(2000);//occupy micro controller for 2S
    state_s1=3;//go to state 3
    break;

    
    case 2:
    //depart
    myDFPlayer.playMp3Folder(2);// play the second mp3 file
    delay(2000);
    state_s1=3;
    break;

    
    case 3:
    myDFPlayer.pause();// pause the miniplayer
    state_s1=0;
    if(state_curr!=state_prev){
    state_prev=state_curr;
    }
    else{
      state_s1=0;
    }
    break;
  }
        }

           else{
        Serial.println("Error occurred!");
    }

    
}



 
