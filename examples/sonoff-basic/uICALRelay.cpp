/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#include "uICALRelay.h"
#include "uICALRelay_config.h"

#include "uICAL/logging.h"


void uICALRelay::begin() {
    for (this->gateCount=0; this->gateCount<100; this->gateCount++) {
        if (!this->gates[this->gateCount].name) {
            break;
        }
    }

    pinMode(this->pushButtonPin, INPUT);
    digitalWrite(this->pushButtonPin, HIGH);

    pinMode(this->statusLedPin, OUTPUT);
    digitalWrite(this->statusLedPin, HIGH);

    for (int i=0; i<this->gateCount; i++) {
        digitalWrite(this->gates[i].pin, LOW);
        pinMode(this->gates[i].pin, OUTPUT);
    }
}

void uICALRelay::statusLed(bool state) {
    digitalWrite(this->statusLedPin, !state);
}

void uICALRelay::statusLedToggle() {
    uint8_t state = digitalRead(this->statusLedPin);
    digitalWrite(this->statusLedPin, !state);
}

void uICALRelay::updateCalendar(Stream& stm) {
    try {
        uICAL::istream_Stream istm(stm);
        this->cal = this->cal = uICAL::Calendar::load(istm, [=](const uICAL::VEvent& event){
          return this->addEvent(event);
        });
    }
    catch (uICAL::Error ex) {
        log_error("%s: %s", ex.message.c_str(), "! Failed loading calendar");
    }
}

bool uICALRelay::addEvent(const uICAL::VEvent& event) {
    for (int i=0; i<this->gateCount; i++) {
        log_debug("gate.name: %s", this->gates[i].name);
        log_debug("summary: %s", event.summary.c_str());
        if (event.summary.indexOf(this->gates[i].name) != -1) {
            return true;
        }
    }
    return false;
}

unsigned uICALRelay::updateGates(unsigned unixTimeStamp) {
    unsigned sleep = this->pollPeriod;

    if (!this->cal) {
        return sleep / 10;
    }

    if (unixTimeStamp == 0) {
        log_warning("%s", "No NTP fix");
        return sleep / 10;
    }
    uICAL::DateTime now(unixTimeStamp);
    uICAL::DateTime calBegin(unixTimeStamp);
    uICAL::DateTime calEnd(unixTimeStamp + this->pollPeriod);

    log_info("calBegin Time: %s", calBegin.as_str().c_str());
    log_info("calEnd Time: %s", calEnd.as_str().c_str());

    log_info("Current Time: %s", now.as_str().c_str());

    try {
        uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(this->cal, calBegin, calEnd);

        uint8_t relayState[this->gateCount];

        for (int i=0; i<this->gateCount; i++) {
            relayState[i] = LOW;
        }

        while (calIt->next()) {
            uICAL::CalendarEntry_ptr entry = calIt->current();
            log_debug("Event @ %s -> %s : %s",
                      entry->start().as_str().c_str(),
                      entry->end().as_str().c_str(),
                      entry->summary().c_str());

            uICAL::DateTime dtStart = entry->start();
            String summary = entry->summary();
            summary.toLowerCase();

            for (int i=0; i<this->gateCount; i++) {
                String relayName = this->gates[i].name;
                relayName.toLowerCase();
                if (entry->start() <= now && now < entry->end()) {
                    if (summary.indexOf(relayName) != -1) {
                        relayState[i] = HIGH;
                        sleep = std::min(sleep, (unsigned)(entry->end() - now).totalSeconds());
                    }
                }
                else if (now < entry->start()) {
                    sleep = std::min(sleep, (unsigned)(entry->start() - now).totalSeconds());
                }
            }
        }

        for (int i=0; i<this->gateCount; i++) {
            uint8_t state = digitalRead(this->gates[i].pin);

            if (relayState[i] != state) {
                digitalWrite(this->gates[i].pin, relayState[i]);
            }
            log_info("Relay: %s - %s", (relayState[i] == HIGH ? "ON  " : "OFF "), this->gates[i].name);
        }
    }
    catch (uICAL::Error ex) {
        log_error("%s", ex.message.c_str());
    }
    return sleep;
}

void uICALRelay::wait(unsigned sleep) {
    log_info("Wait: %ds", sleep);

    sleep *= 1000;
    while (sleep > 0) {
        delay(200);
        sleep -= 200;
        bool update = digitalRead(this->pushButtonPin);
        if (!update) {
            log_info("%s", "Update button pressed");
            break;
        }
    }
}
