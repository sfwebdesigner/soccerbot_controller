#ifndef JOYSTICK_H
#define JOYSTICK_H

class Joystick
{
public:
    Joystick(int xPinIn, int yPinIn, int trigPinIn);
    void tick();
    void readCenter();
    int getX();
    int getY();
    int getTrig();

protected:
    int tickTimerMsecs;
    int xPin;
    int yPin;
    int trigPin;
    long scale(long val);

    long x;
    long y;
    int trig;

    // two stage history for debouncing.  
    int trig_new;
    int trig_prev;

    // reading taken at power up to determine the centered joystick value
    long x_center; 
    long y_center;
};

#endif