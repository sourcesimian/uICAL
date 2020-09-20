/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

#if defined(ARDUINO_ARCH_ESP8266)
    #include <ESP8266WiFi.h>
#else
    #include <Arduino.h>
#endif

#include <functional>
#include "ButtonMonitor.h"

ButtonMonitor::ButtonMonitor(uint8_t pin)
: pin(pin)
, pressMillis(0)
{
    digitalWrite(pin, HIGH);
    pinMode(pin, INPUT);
}

long ButtonMonitor::getPressed() {
    if (isPressed()) {
        if (!pressMillis) {
            pressMillis = millis();
            return 0;
        }
        return pressMillis - millis();
    }
    else {
        if (pressMillis) {
            unsigned ret = millis() - pressMillis;
            pressMillis = 0;
            return ret;
        }
        return 0;
    }
}

bool ButtonMonitor::isPressed() {
    uint8_t state = digitalRead(pin);
    return state == 0;
}
