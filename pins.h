#ifndef PINS_H
#define PINS_H

// battery sense analog pin
#define BATTERY_SENSE A0

// joystick analog pins
#define JS_LEFT_X  A7
#define JS_LEFT_Y  A6
#define JS_RIGHT_X A3
#define JS_RIGHT_Y  A2

// joystic digital pins
#define JS_LEFT_TRIG 8
#define JS_RIGHT_TRIG 9

// define dip switch outputs/inputs
#define DS_BIT_0 2
#define DS_BIT_1 3
#define DS_BIT_2 4
#define DS_BIT_3 5
#define DS_BIT_4 6
#define DS_BIT_5 7

// define NRF pins
#define NRF_MISO 12
#define NRF_MOSI 11
#define NRF_CS 10
#define NRF_IRQ 2
#define NRF_SCK 13

// note serial tx0 is used to drive indicator led


#endif
