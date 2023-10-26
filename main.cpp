#include "pins.h"
#include "dipsw.h"
#include "joystick.h"
#include "battery.h"
#include "nrfController.h"
#include "led.h"
#include "msecTimer.h"

#include <arduino-timer.h>

//===========================================================   VERSION
//  1.03 - previous version 1.02 did not transmit packets.  This version
//         corrects the problem (06/01/2012)
//  1.04 - 


//============================================================  JOYSTICKS and TRIGGER SWTICHES
//
// This code labels the joystick based on the labels on the PCB.  However
// when the joysticks are mounted in the controller case the one on the right
// connects to the lJoy inputs and the one of the left the rJoy inputs.  The same
// is true for the trigger switches: lTrig is the right switch and rTrig is the
// left swtich
//
// Futhermore, the joystics are rotated -90 degress, so the X signal ends
// being up and down joystick motion and the Y signal ends up being left
// and right motion
//
// Finally, the voltage reading of the joystick is non-inverted.  So moving
// the joystick left of down produces the smallest reading and right or up
// produces the largest reading.
//
// Instantiate joystick/tigger object to match physical positioning
//
//               horz,      vert,      trigger signal pins
Joystick jsRight(JS_LEFT_Y, JS_LEFT_X, JS_LEFT_TRIG);   // configure for actual joystick physical positioning (left signals are right joystick and trigger, x is really y and visa versa)
Joystick jsLeft(JS_RIGHT_Y, JS_RIGHT_X, JS_RIGHT_TRIG); // configure for actual joystick physical positioning (right signals are left joystick and trigger, x is really y and visa versa)

//============================================================ OTHER COMPONENTS
//
Led led;
Dipsw dipsw;
Battery battery;
NrfController nrf(10, A1, 2000000);   // rcsPin, csnPin, spiSpeed in MHZ

unsigned long diptimer = millis();
unsigned long mainTaskTimer = millis();
unsigned char dip;

char buff[256];

// point to task we want to perform in loop
void (*task)();


// prototypes for tasks
void mainTask();
void DipSwitchTestStart();
void DipSwitchTestLoop();
void dispatchTests();
void lJoyVertTest();
void lJoyHorzTest();
void rJoyVertTest();
void rJoyHorzTest();
void nrfTest();

//================================================================================ 1 msec interrupt
// Interrupt is called once a millisecond,
SIGNAL(TIMER0_COMPA_vect) {
  // blink led using TX0 line
  led.tick();
}

//================================================================================ setup
void setup() {
  led.begin(1200);

  // let everything settle
  delay(10);

  // assume joysticks are centered
  // read and save the centered value
  jsLeft.readCenter();
  jsRight.readCenter();

  // todo get dip switches so we can set channel
  int chan = dipsw.read() & 0x3F;
  nrf.begin(chan);

  // start out doing mainTask in loop
  task = &mainTask;

  // led blinking is achieved by calling the led.tick() method periodically
  // but we can't do this from the loop as its loop time varies when using the
  // NRF chip.
  // So instead we want to call led.tick() from the interrupt level.
  // Timer0 is already used for millis(), here we set Timer0 up to
  // SIGNAL(TIMER0_COMPA_vect) which in turn calls led.tick.
  OCR0A = 0xAF;
  TIMSK0 |= _BV(OCIE0A);

}
//================================================================================ loop

void loop() {
  // process joysticks and trigger switches
  jsLeft.tick();
  jsRight.tick();

  // perform the current task (either mainTask or one of the self test tasks)
  task();
}

//=============================================================================== TASKS
//
int lTrig_setLedBlink = false;
int lTrig_DepressedStartMsecs = 0;
int lTrigPrev = 0;
int rTrig_DepressedStartMsecs = 0;
int rTrigPrev = 0;
int mainTaskPeriodInMsecs = 20;
int testModeSwitchTime = 4000;
MsecTimer tmr_MainTask(mainTaskPeriodInMsecs);

//================================================================================ regular mode
//
int mainTask_idleCount = 0;   // used to slow down transmits when joysticks are centered and switches are not depressed/
void mainTask () {
  // swread battery, joysticks and trigger switches frequently
  if (tmr_MainTask.timeout()) {

    // swread battery voltage
    int bvoltage = battery.readMilliVolts();

    // poll left joystick and trigger switch
    int lx = jsLeft.getX();
    int ly = jsLeft.getY();
    int lTrig = jsLeft.getTrig();

    // switch just depressed
    if (!lTrigPrev && lTrig) {
      lTrig_DepressedStartMsecs = millis();
    }
    // switch released
    if (!lTrigPrev && !lTrig) {
      lTrig_DepressedStartMsecs = 0;
      lTrig_setLedBlink = false;
    }
    lTrigPrev = lTrig;

    // poll right joystick and trigger switch
    int rx = jsRight.getX();
    int ry = jsRight.getY();
    int rTrig = jsRight.getTrig();

    // switch just depressed
    if (!rTrigPrev && rTrig) {
      rTrig_DepressedStartMsecs = millis();
    }
    // switch released
    if (!rTrigPrev && !rTrig) {
      rTrig_DepressedStartMsecs = 0;
    }
    rTrigPrev = rTrig;

    // Are switches/joystics in idle position
    if (lTrig == 0 && rTrig == 0 &&
        abs(lx) < 5 &&
        abs(ly) < 5 &&
        abs(rx) < 5 &&
        abs(ry) < 5) {
      // yes
    }

    // transmit joystick/trig info to vehicle
    NrfPacket packet;
    packet.lJoyX = lx;
    packet.lJoyY = ly;
    packet.lTrig = lTrig;
    packet.rJoyX = rx;
    packet.rJoyY = ry;
    packet.rTrig = rTrig;
    nrf.write(packet);


    // when trigger switches are not depressed
    // set led tick rate depending on bvoltage
    // > 3.9 on steady
    // > 3.6 medium fast blink
    // otherwise really fast blink
    //
    if (!lTrig && !rTrig) {
      if (bvoltage >= 3900)
        led.setBlinkRateInMsecs(0);  // on steady
      else if (bvoltage > 3600)
        led.setBlinkRateInMsecs(500); // flash slow
      else
        led.setBlinkRateInMsecs(50);  // flash quick
    }
    else {
      // one or both trigger switches are depressed
      // enter test mode ?
      if (lTrig_DepressedStartMsecs > 0 ) {
        if (millis() - lTrig_DepressedStartMsecs > testModeSwitchTime) {
          if (!lTrig_setLedBlink) {
            led.setBlinkRateInMsecs(333);
            lTrig_setLedBlink = true;
          }
          if (rTrig_DepressedStartMsecs > 0 ) {
            if (millis() - rTrig_DepressedStartMsecs > testModeSwitchTime) {
              task = &dispatchTests;
            }
          }
        }
      }
    }
  }
}

//===============================================================================  Dispatch Tests
//
MsecTimer tmr_dispatchTests(100);
void dispatchTests() {
  if (tmr_dispatchTests.timeout()) {

    // read dipswitch
    int swread1, swread2, swread3;
    do {
      swread1 = dipsw.read();
      delay(1);
      swread2 = dipsw.read();
      delay(1);
      swread3 = dipsw.read();
    } while ((swread1 != swread2) || (swread2 != swread3));

    switch (swread1) {
      case 0:
        led.setBlinkRateInMsecs(500);
        break;
      case 0x01:
        lJoyVertTest();   // actually right joystick when mounted in the controller case and this is horz
        break;
      case 0x03:
        lJoyHorzTest();   // actually right joystick when mounted in the controller case and this is vert
        break;
      case 0x07:
        rJoyVertTest();   // actually left joystick when mounted in the controller case and this is horz
        break;
      case 0x0F:
        rJoyHorzTest();   // actually left joystick when mounted in the controller case and this is vert
        break;
      case 0x1F:
        nrfTest();
        break;
    }
  }
}


//===============================================================================  Dip Switch Test
//
MsecTimer tmr_testMode_DipSwitch(1000);
bool testMode_DipSwitchMaxReached;

void DipSwitchTestStart () {
  led.setBlinkRateInMsecs(50);

  int ltrig = jsLeft.getTrig();
  int rtrig = jsRight.getTrig();
  if (!ltrig && !rtrig) {
    task = &DipSwitchTestLoop;
    tmr_testMode_DipSwitch.startTimeout();
    bool testMode_DipSwitchMaxReached = false;
  }
}

void DipSwitchTestLoop() {
  if (tmr_testMode_DipSwitch.timeout()) {
    // read dipswitch
    int swread = dipsw.read();

    // set blink rate
    led.setBlinkRateInMsecs(10 + swread * 25);

    if (swread >= 0x20) testMode_DipSwitchMaxReached = true;
    if (testMode_DipSwitchMaxReached && swread == 0) {
      // indicate exit from dip switch test
      led.setBlinkRateInMsecs(30);

      // start new task
      task = &dispatchTests;
      // delay dispatchTests first timeout
      tmr_dispatchTests.startDelayedTimeout(1000);  // timeout 1 second from now
    }
  }
}

//=========================================================================================== joystick test
void lJoyVertTest() {

  unsigned int blinkRate =  125;
  int ly = jsLeft.getY();
  led.setBlinkRateInMsecs(blinkRate - ly);

}

void lJoyHorzTest() {

  unsigned int blinkRate = 125;
  int lx = jsLeft.getX();
  led.setBlinkRateInMsecs(blinkRate - lx);

}

void rJoyVertTest() {

  unsigned int blinkRate = 125;
  int ry = jsRight.getY();
  led.setBlinkRateInMsecs(blinkRate - ry);

}

void rJoyHorzTest() {
  unsigned int blinkRate = 125;
  int rx = jsRight.getX();
  led.setBlinkRateInMsecs(blinkRate - rx);
}


//=========================================================================================== NRF Test
MsecTimer tmr_nrfTest(1000);
void nrfTest() {
  if (tmr_nrfTest.timeout()) {
    // can we commnication with the NRF module via the SPI interface
    // if so the begin method will return true
    if (nrf.begin(1)) {
      // we can communicate with NRF via SPI
      //Serial.println("NRF ok");
      led.setBlinkRateInMsecs(500);
    }
    else {
      // oops we can't communicate
      //Serial.println("NRF failed");
      led.setBlinkRateInMsecs(50);
    }
  }
}
