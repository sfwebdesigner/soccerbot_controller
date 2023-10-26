#include "msecTimer.h"
#include <Arduino.h>

//=========================================================== constructor
MsecTimer::MsecTimer(unsigned int _timeoutMsec, bool startNow)
{
    timeoutMsecs = _timeoutMsec;
    enabled = false;
    if (startNow)
        startTimeout();
}

//============================================================ setTripTimeMsecs
unsigned int MsecTimer::getTripTimeMsecs()
{
    return timeoutMsecs;
}

//============================================================ setTripTimeMsecs
void MsecTimer::setTimeoutMsecs(unsigned int _timeoutMsecs)
{
    timeoutMsecs = _timeoutMsecs;
}

//=========================================================== timeout
bool MsecTimer::timeout()
{
    if (!enabled)
        return false;                       // return if timeout not enabled

    elapsedTimeMsecs = millis() - startTimeMsecs;

    // is there a delay before we start the actual timeout
    if (delayTimeMsecs)
    {
        // yes, then have we delayed long enough
        if (elapsedTimeMsecs > delayTimeMsecs)
        {
            // yes, start timeout from right now
            delayTimeMsecs = 0;
            startTimeMsecs = millis();
        }
    }
    else
    {
        if (elapsedTimeMsecs >= timeoutMsecs)
        {
            startTimeMsecs = millis();
            return true;                    // return true when timeout reached
        }
    }

    return false;                           // return false when timeout not reached
}

//=========================================================== start timeout
void MsecTimer::startTimeout()
{
    startTimeMsecs = millis();
    delayTimeMsecs = 0;
    elapsedTimeMsecs = 0;
    enabled = true;
}
void MsecTimer::startDelayedTimeout(unsigned int _delayTimeMsecs)
{
    startTimeout();
    delayTimeMsecs = _delayTimeMsecs;
}

//=========================================================== stop timeout
void MsecTimer::stop()
{
    enabled = false;
}
