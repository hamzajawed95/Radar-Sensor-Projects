/***********Includes*************/
#include <Arduino.h>
/* Include library main header */
#include <bgt60-ino.hpp>
/* Include Arduino platform header */
#include <bgt60-platf-ino.hpp>

#include <SPI.h>
#include <WiFiNINA.h>//wifi library for mkrwifi 1010
#include <utility/wifi_drv.h>//library for onboard LED control
#include <BlynkSimpleWiFiNINA.h>

/*********************** wifi credentials *********************/
char auth[] = " ls4DunOd0FgHh3LfwCxG_NPM4Von0-AK";//authentication token for the app
char ssid[] = "iPhone";
char pass[] = "hjhjhjhj";


/***********Defines*************/
#ifndef TD
#define TD  15
#endif

#ifndef PD
#define PD  16
#endif

/*********************** variables *********************/
int GreenLED=25;//Pin number for onboard Green LED
int RedLED=26;//Pin number for onboard Red LED
int BlueLED=27;//Pin number for onboard Blue LED
int Led=7;
WidgetLED ledrad(V2);// you can change pin number here to match the one assigned in widget box)


/* Create radar object with following arguments:
 *  TD : Target Detect Pin
 *  PD : Phase Detect Pin */
Bgt60Ino radarShield(TD, PD);

void setup()
{
   Serial.begin(9600);
   Blynk.begin(auth, ssid, pass);  // Start communication with Blynk App

   WiFiDrv::pinMode(RedLED, OUTPUT); //syntax defining MKR wifi1010 onboard LED as OUTPUT.
   WiFiDrv::pinMode(BlueLED, OUTPUT); 
   pinMode(Led,OUTPUT);

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


 
void loop()
{
    /* Initialize the variable to NO_DIR to be able to record new events */
    Bgt60::Direction_t direction = Bgt60::NO_DIR;

    /* The getDirection() API does two things:
        1. Returns the success or failure to detect direction of object as a message of type Error_t.
           Any value other than OK indicates failure
        2. Sets recent event in "direction" variable. Events can be: APPROACHING, DEPARTING or NO_DIR */
    Error_t err = radarShield.getDirection(direction);      

    /* Check if API execution is successful */
    if (err == OK)
    {   
        digitalWrite(Led,LOW);
        /* Cases based on value set in direction variable */
        switch (direction)
        {
            /* Variable "direction" is set to APPROACHING when target is moving closer to sensor */
            case Bgt60::APPROACHING:
                WiFiDrv::digitalWrite(BlueLED, HIGH);//blue on board LED switches ON
                WiFiDrv::digitalWrite(RedLED, LOW);//Red on board LED switches OFF
                ledrad.on();
                break;
            /* Variable "direction" is set to DEPARTING when target is moving away from sensor */
            case Bgt60::DEPARTING:
                WiFiDrv::digitalWrite(RedLED, HIGH);//Red on board LED switches ON
                WiFiDrv::digitalWrite(BlueLED, LOW);//blue on board LED switches OFF 
                ledrad.on();
                break;
            /* Variable "direction" is set to NO_DIR when no motion was detected */
            case Bgt60::NO_DIR:
                WiFiDrv::digitalWrite(RedLED, LOW);//Red on board LED switches OFF
                WiFiDrv::digitalWrite(BlueLED, LOW);//blue on board LED switches OFF
                delay(1000); 
                digitalWrite(Led,HIGH);
                ledrad.off(); 
                break;
        }
    }
    /* API execution returned error */
    else{
        Serial.println("Error occurred!");
    }

    /* Reducing the frequency of the measurements */
    //delay(500);
}
