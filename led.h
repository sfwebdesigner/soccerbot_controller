#ifndef LED_H
#define LED_H

class Led
{
public:
    Led();
    void begin(int baudRateIn);
    void tick();
    void setBlinkRateInMsecs(int msecs);

protected:
    int state;
    int count;
    int baudRate;
    int byteRateInMsecs;
    int blinkPeriodInMsecs;
    int blinkCount;
};

#endif