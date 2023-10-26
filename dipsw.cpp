#include "pins.h"
#include "dipsw.h"
#include <Arduino.h>

//======================================================= constructor
Dipsw::Dipsw()
{
   pinMode(DS_BIT_0, INPUT_PULLUP);
   pinMode(DS_BIT_1, INPUT_PULLUP);
   pinMode(DS_BIT_2, INPUT_PULLUP);
   pinMode(DS_BIT_3, INPUT_PULLUP);
   pinMode(DS_BIT_4, INPUT_PULLUP);
   pinMode(DS_BIT_5, INPUT_PULLUP);

}

//====================================================== read
unsigned char Dipsw::read()
{
    unsigned char result = 0xC0;

    result |= digitalRead(DS_BIT_0);
    result |= digitalRead(DS_BIT_1) << 1;
    result |= digitalRead(DS_BIT_2) << 2;
    result |= digitalRead(DS_BIT_3) << 3;
    result |= digitalRead(DS_BIT_4) << 4;
    result |= digitalRead(DS_BIT_5) << 5;

    return ~result;
}
