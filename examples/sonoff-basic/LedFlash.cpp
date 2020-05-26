/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
#else
    #include <Arduino.h>
#endif

#include "LedFlash.h"


LedFlash::LedFlash(uint8_t pin)
: pin(pin)
, flashOn(0)
, flashOff(0)
, stateChangeMillis(0)
{
    digitalWrite(pin, HIGH);
    pinMode(pin, OUTPUT);
}


void LedFlash::flash(int on, int off) {
    if (off == 0) {
        flashOn = on;
        flashOff = on;
    }
    else {
        flashOn = on;
        flashOff = off;
    }
    
    toggle();
    stateChangeMillis = millis();
}


void LedFlash::state(bool on) {
    flashOn = 0;
    flashOff = 0;
    setState(on);
}

void LedFlash::toggle() {
    setState(!getState());
    stateChangeMillis = millis();
}

void LedFlash::setState(bool on) {
    digitalWrite(pin, !on);
}

bool LedFlash::getState() {
    uint8_t state = digitalRead(pin);
    return state == 0;
}

void LedFlash::handle() {
    if (flashOn) {
        unsigned now = millis();
        if ((now - stateChangeMillis) > (getState() ? flashOn : flashOff)) {
            toggle();
        }
    }
}
