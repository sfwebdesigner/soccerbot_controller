#ifndef NRF_PACKET_H
#define NRF_PACKET_H

class NrfPacket {
    public:
        int16_t lJoyX;  
        int16_t lJoyY;
        int16_t lTrig;
        int16_t rJoyX;
        int16_t rJoyY;
        int16_t rTrig;   
        byte pad[20];   
};

#endif
