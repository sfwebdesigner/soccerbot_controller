#ifndef NRF_H
#define NRF_H

#include <RF24.h>
#include "nrfPacket.h"


class NrfController
{
public:
    NrfController(int csPin, int csnPin, long spiSpeed);
    bool begin(int _channelNum);
    void write(NrfPacket Xmit);

protected:
    RF24 radio;
    int  channelNum;
};


#endif
