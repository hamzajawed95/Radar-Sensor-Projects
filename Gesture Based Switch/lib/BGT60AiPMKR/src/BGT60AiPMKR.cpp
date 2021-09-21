/// @file BGT60AiPMKR.h
/// @author Geike Hannes Mathias (IFAG PSS RFS 3DS RS PM) <HannesMathias.Geike@infineon.com>
/// @date 11-02-2020
/// @version V0.0

#include"BGT60AiPMKR.h"
#include<Arduino.h>

BGT60AiPMKR::BGT60AiPMKR(void) : enPin{-1}
{
    tdInfo = new PinInfo(DEFAULT_TD_PIN, ANALOG, false, false, 0);
    pdInfo = new PinInfo(DEFAULT_PD_PIN, ANALOG, false, false, 0);
    isEnabled = true;
    setADCResolution(DEFAULT_ADC_RES);
    this->initPinValues();
}

BGT60AiPMKR::BGT60AiPMKR(unsigned char tdpin, unsigned char pdpin)
    : enPin(-1)
{
    tdInfo = new PinInfo(tdpin, DIGITAL, false, false, 0);
    pdInfo = new PinInfo(pdpin, DIGITAL, false, false, 0);
    isEnabled = true;
    pinMode(tdInfo->pinNumber, INPUT);
    pinMode(pdInfo->pinNumber, INPUT);
    this->initPinValues();
}

BGT60AiPMKR::BGT60AiPMKR(unsigned char tdpin, unsigned char pdpin, unsigned char enpin)
    : enPin(enpin)
{
    tdInfo = new PinInfo(tdpin, DIGITAL, false, false, 0);
    pdInfo = new PinInfo(pdpin, DIGITAL, false, false, 0);
    isEnabled = true;
    pinMode(tdInfo->pinNumber, INPUT);
    pinMode(pdInfo->pinNumber, INPUT);
    pinMode(enPin, OUTPUT);
    enable();
    this->initPinValues();
}

void BGT60AiPMKR::setIsEnabled(bool ie)
{
    if (enPin != -1)
    {
    /* Set enable pin and inforamtion */
        digitalWrite(enPin, !ie); // Low--active
        isEnabled = ie;
    }
    else
    {
    /* isEnabled always true if no switch connected */
        isEnabled = true;
    }
}

void BGT60AiPMKR::enable(void)
{
    setIsEnabled(true);
}

void BGT60AiPMKR::disable(void)
{
    setIsEnabled(false);
}

void BGT60AiPMKR::initPinValues(void)
{
    this->read();   // Update pin info
    // Mark pin info as first read-out
    tdInfo->timeStamp = 0;
    tdInfo->hasChanged = false;
    pdInfo->timeStamp = 0;
    pdInfo->hasChanged = false;
}

bool BGT60AiPMKR::read()
{   
    bool bRet = false;
    if (isEnabled)
    {
        /* Get TD pin information */
        switch(tdInfo->pinType)
        {
        case ANALOG:
                this->readTdInfoAnalog();
            break;

        case DIGITAL:
                this->readTdInfoDigital();
            break;
        }

        /* Get PD pin information */
        switch(pdInfo->pinType)
        {
        case ANALOG:
                this->readPdInfoAnalog();
            break;

        case DIGITAL:
                this->readPdInfoDigital();
            break;
        }

        bRet = true;    // Mark pin value read as successful
    }
    return bRet;
}

void BGT60AiPMKR::readTdInfoAnalog(void)
{
    bool tdval = false;
    if(analogRead(tdInfo->pinNumber) > analogTh)
    {
        tdval = true;
    }

    if (tdval != tdInfo->currValue)
    {
        tdInfo->hasChanged = true;
        tdInfo->currValue = tdval;
        tdInfo->timeStamp = millis();
    }
}

void BGT60AiPMKR::readTdInfoDigital(void)
{
    bool tdval = false;
    tdval = digitalRead(tdInfo->pinNumber);

    if (tdval != tdInfo->currValue)
    {
        tdInfo->hasChanged = true;
        tdInfo->currValue = tdval;
        tdInfo->timeStamp = millis();
    }
}

void BGT60AiPMKR::readPdInfoAnalog(void)
{
    bool pdval = false;
    if(analogRead(pdInfo->pinNumber) > analogTh)
    {
        pdval = true;
    }

    if (pdval != pdInfo->currValue)
    {
        pdInfo->hasChanged = true;
        pdInfo->currValue = pdval;
        pdInfo->timeStamp = millis();
    }
}

void BGT60AiPMKR::readPdInfoDigital(void)
{
    bool pdval = false;
    pdval = digitalRead(pdInfo->pinNumber);

    if (pdval != pdInfo->currValue)
    {
        pdInfo->hasChanged = true;
        pdInfo->currValue = pdval;
        pdInfo->timeStamp = millis();
    }
}

void BGT60AiPMKR::setADCResolution(unsigned res)
{
    if((tdInfo->pinType == ANALOG) || (pdInfo->pinType == ANALOG))
    {
    /* At least one pin is analog to prevent unnecessary change of pin resolution */
        analogRes = res;
        analogReadResolution(analogRes);    // Set Arduinos ADC resolution
        this->setADCThreshold();
    }
}

void BGT60AiPMKR::setADCThreshold()
{
    int range = pow(2, analogRes);
    analogTh = (range/2) - 1;
}

bool BGT60AiPMKR::getTdValueWReset()
{ 
    tdInfo->hasChanged = false;
    return tdInfo->currValue; 
}

bool BGT60AiPMKR::getPdValueWReset()
{ 
    pdInfo->hasChanged = false;
    return pdInfo->currValue; 
}