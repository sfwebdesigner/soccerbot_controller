#include "led.h"
#include <Arduino.h>

//============================================================= constructor
Led::Led()
{

}

//============================================================= begin
void Led::begin(int baudRateIn)
{
    baudRate = baudRateIn;
    Serial.begin(baudRate); 
    byteRateInMsecs = 1000/(baudRate/10);

    setBlinkRateInMsecs(500);
    //Serial.println(byteRateInMsecs);
    //Serial.println(blinkPeriodInMsecs);
    //Serial.println(blinkCount);

    state = 0;
    count = 0;
}

//============================================================= setBlinksPerSecond
void Led::setBlinkRateInMsecs(int msec) {
    blinkPeriodInMsecs = msec;
    blinkCount = blinkPeriodInMsecs/byteRateInMsecs;
}

//============================================================= tick
void Led::tick()
{
    // solid on?
    if (blinkPeriodInMsecs == 0)
        // leave serial alone
        return;
        
    //  does serial buffer has one spot
    if (Serial.availableForWrite() >= 63)
    {
        // yes send either 0 of 0xff (state)
        Serial.write(state);

        // time to toggle
        if (++count >= blinkCount)
        {
            if (state == 0)
                state = 0xFF;
            else
                state = 0;

            count = 0;
        }
    }
}
