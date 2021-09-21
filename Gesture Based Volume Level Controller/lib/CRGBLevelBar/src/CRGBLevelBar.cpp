/// @file CRGBLevelBar.cpp
/// @author Geike Hannes Mathias (IFAG PSS RFS 3DS RS PM) <HannesMathias.Geike@infineon.com>
/// @date 11-02-2020
/// @version V0.0

#include<Arduino.h>
#include"CRGBLevelBar.h"

uint32_t G2B10Scheme[10] = {0xE5BC00, 
    0xE77600, 0xEA2E00, 0xED001B, 
    0xF00067, 0xF300B5, 0xE700F6, 
    0x9C00F9, 0x4F00FC, 0x0000FF};

uint32_t B2G10Scheme[10] = {0x0000FF, 
    0x4F00FC, 0x9C00F9, 0xE700F6, 
    0xF300B5, 0xF00067, 0xED001B, 
    0xEA2E00, 0xE77600, 0xE5BC00};

uint32_t W10Scheme[10] = {0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF, 
    0xFFFFFF, 0xFFFFFF, 0xFFFFFF};

CRGBLevelBar::CRGBLevelBar() : barSize{10}, dataPin{5}
{
    colorScheme = G2B10Scheme;
    schemeSize = 10;
    brightness = (uint8_t)10;
    /// @todo Fix pin number issue
    FastLED.addLeds<WS2812, 5>(ledArray, barSize, 0);
}

CRGBLevelBar::CRGBLevelBar(uint8_t barsize, uint8_t datapin)
    : barSize{barsize}, dataPin{datapin}
{
    colorScheme = G2B10Scheme;
    schemeSize = 10;
    brightness = (uint8_t)10;
    /// @todo Fix pin number issue
    FastLED.addLeds<WS2812, 5>(ledArray, barSize);
}

void CRGBLevelBar::turnOff(void)
{
    memset(ledArray, 0, barSize * 3);
    FastLED.show();
}

void CRGBLevelBar::test(void)
{
    for(int s = 0; s < 2; s++)
    {
        for(int j = 0; j < 3; j++) {
            for(unsigned i = 0 ; i < barSize; i++) {
                switch(j) {
                    case 0: 
                        ledArray[i].r = (uint8_t)(0xFF*((float)(brightness/100.0))); 
                    break;
                    case 1: 
                        ledArray[i].g = (uint8_t)(0xFF*((float)(brightness/100.0))); 
                    break;
                    case 2: 
                        ledArray[i].b = (uint8_t)(0xFF*((float)(brightness/100.0))); 
                    break;
                }
                FastLED.show();
            }
        }
        FastLED.show();
        delay(500);
        turnOff();
        delay(500);
    }
}

void CRGBLevelBar::setValue(uint8_t val){
    value = val;
    float brightnessFactors[barSize] = {0};
    float generalBrightness = (float)brightness/100.0;
    /* Calculate brightness factors */
    
    uint8_t fullLeds = floor(value/barSize);
    for(uint8_t i = 0; i < fullLeds; i++)
    {   
    /** Full brightness for integral multiple of value **/
        brightnessFactors[i] = generalBrightness;
    }
    /** Brighntess of remaing LED **/
    if (fullLeds < barSize)
    {
        brightnessFactors[fullLeds] = 
            ((value-(fullLeds*barSize))/(float)barSize) * // Calculate pixel brighntess
            generalBrightness;                            // Respect general brightness
    }

    /* Prepare LED values  */
    for(int j = 0; j < 3; j++) {
        for(uint8_t i = 0 ; i < barSize; i++) {
            switch(j) {
                case 0: 
                    ledArray[i].r = (uint8_t)(((0xFF0000 & colorScheme[i]) >> 16) 
                        * brightnessFactors[i]);
                break;
                case 1: 
                    ledArray[i].g = (uint8_t)(((0x00FF00 & colorScheme[i]) >> 8)
                        * brightnessFactors[i]);
                    break;
                case 2: 
                    ledArray[i].b = (uint8_t)((0x0000FF & colorScheme[i])
                        * brightnessFactors[i]);
                break;
            }
            FastLED.show();
        }
    }
    FastLED.show();
}