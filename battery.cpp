#include "pins.h"
#include "battery.h"
#include <Arduino.h>

//======================================================== constructor
Battery::Battery() {

}

//======================================================== readVoltage
int Battery::readMilliVolts() {

    // reading will range from 0 to 1023 (10 bit A/D)
    // representing 0 to 3.3volts
    long reading =  analogRead(BATTERY_SENSE);
    long volts = reading * 3300 /1024;

    // volts is the reading at the voltage divider ( bat - 10k - 33k - gnd)
    // calculate the voltage going into the voltage divider
    // to get battery voltage.   

    volts = volts*43/33;  // assume perfect resistors in voltage divider

    return volts;
}