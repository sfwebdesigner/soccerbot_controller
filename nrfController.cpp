#include "pins.h"
#include "nrfController.h"
#include <Arduino.h> 

#include <RF24.h>


#define PALevel RF24_PA_MAX
#define XMIT_PIPE 0xF0F0F0F0F0LL 
#define RCV_PIPE 0xF0F0F0F0F0LL

//==================================================================== constructor
NrfController::NrfController (int csPin, int csnPin, long spiSpeed) :
     radio(csPin, csnPin, spiSpeed)
{
}

//==================================================================== controller begin
bool NrfController::begin(int _channelNum) {
    // start up the radio
    if (!radio.begin()) 
        // return now if failed
        return false;
    
    // radio is good, so configure its settings

    // select the carrier frequency (channel)
    channelNum = _channelNum;
    radio.setChannel(channelNum);

    // set the transmitter power level
    radio.setPALevel(PALevel);  // RF24_PA_MAX is default. 

    // set data rate
    radio.setDataRate(RF24_1MBPS);

    // set xmit pipe and go into transmit mode
    radio.openWritingPipe(XMIT_PIPE);     // controller and vehicle use just one pipe
    // set transmit payload size
    //radio.setPayloadSize(sizeof(NrfPacket)); 
    radio.stopListening();  // put radio in tx mode

    return true;
}

//==================================================================== write (xmit used by controller)
void NrfController::write(NrfPacket Xmit) {
   radio.write(&Xmit, sizeof(Xmit));
}
