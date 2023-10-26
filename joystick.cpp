#include "pins.h"
#include "joystick.h"
#include <Arduino.h>

//================================================================= constructor
Joystick::Joystick(int xPinIn, int yPinIn, int trigPinIn)
{
    xPin = xPinIn;
    yPin = yPinIn;
    trigPin = trigPinIn;

    trig_prev = 1;

    // set pin mode for analog inputs
    analogRead(xPin);
    analogRead(yPin);

    // set pin mode to read trig sw
    pinMode(trigPin, INPUT_PULLUP);

    tickTimerMsecs = millis();
};

//================================================================= tick
void Joystick::tick()
{
    // read every millisecond
    if (millis() - tickTimerMsecs > 2)
    {
        tickTimerMsecs = millis();

        // read X and Y and scale/offset values
        x = scale(analogRead(xPin) - x_center);
        y = scale(analogRead(yPin) - y_center);

        // simple debounce 
        // only update trig value if
        // we get two consecutive reads that are the same level
        trig_new = !digitalRead(trigPin);
        if (trig_new == trig_prev) {
            trig = trig_new;
        }
        trig_prev = trig_new;
    }
};

//================================================================= getX
int Joystick::getX()
{
    return x;
};

//================================================================= get Y
int Joystick::getY()
{
    return y;
};

//================================================================= getTrig
int Joystick::getTrig()
{
    return trig;
};

//================================================================= scale
long Joystick::scale(long val) {
    return val * 200 / 1024;   // scale to 0 to 200
}

//================================================================= setZero
void Joystick::readCenter() {
    // determine scaled center value (should be around 100)
    int x_avr = 0;
    int y_avr = 0;
    for (int i = 0; i<10; i++) {
        x_avr += analogRead(xPin);
        y_avr += analogRead(yPin);
        delay(1);
    }
    x_center = x_avr/10;
    y_center = y_avr/10;
}
