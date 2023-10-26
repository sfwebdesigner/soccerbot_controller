#ifndef MSECTIMER_H
#define MSECTIMER_H

class MsecTimer {
    public:
    MsecTimer(unsigned int _timeoutMsec, bool startNow=true);
    unsigned int getTripTimeMsecs();
    void setTimeoutMsecs(unsigned int _timeoutMsecs);
    bool timeout();
    void startTimeout();
    void startDelayedTimeout(unsigned int _delayTimeMsecs);
    void stop();
    
    protected:
        bool enabled;
        unsigned int timeoutMsecs;
        unsigned int startTimeMsecs;
        unsigned int delayTimeMsecs;

    public:
        unsigned int elapsedTimeMsecs;

};

#endif
