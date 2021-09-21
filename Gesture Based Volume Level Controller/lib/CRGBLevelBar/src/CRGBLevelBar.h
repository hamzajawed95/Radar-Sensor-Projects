/// @file CRGBLevelBar.h
/// @author Geike Hannes Mathias (IFAG PSS RFS 3DS RS PM) <HannesMathias.Geike@infineon.com>
/// @date 11-02-2020
/// @version V0.0

#ifndef CRGBLEVELBAR_H
#define CRGBLEVELBAR_H

#include <stdint.h>
#include"FastLED.h"

#define MAX_LED 50

extern uint32_t G2B10Scheme[];
extern uint32_t B2G10Scheme[];
extern uint32_t W10Scheme[];

/// Driver class for CRGB Level Bar using a the controllable LED
/// Driver Type WS2812.
class CRGBLevelBar
{
    private:
        CRGB ledArray[MAX_LED];
        const uint8_t barSize;  /* Pixel size of level bar */
        const uint8_t dataPin;  /* Data pin of */
        uint32_t* colorScheme;  /* Pixel information of color scheme */
        uint32_t schemeSize;    /* Bit resolution of color scheme */
        uint8_t brightness;     /* Brighntess level in Percent (0-100) */
        uint8_t value;          /* Value to be displayed in Percent (0-100) */

    public:
        CRGBLevelBar();
        CRGBLevelBar(uint8_t barsize, uint8_t datapin);

        void test(void);

        uint8_t getBarSize(void) { return barSize; };
        uint8_t getDataPin(void) { return dataPin; };
        uint8_t getValue(void) { return value; }

        void setValue(uint8_t val);
        void setBrightness(uint8_t bs) { brightness = bs; };
        void setColorScheme(uint32_t* cscheme) { colorScheme = cscheme; };

        void turnOff(void);
       
};


#endif /* CRGBLEVELBAR_H */
