/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#include "uICALRelay.h"
#include "uICAL/logging.h"

uICALRelay::uICALRelay(config_t& config,
                       updateCalendar_t updateCalendar,
                       getUnixTimeStamp_t getUnixTimeStamp)
: config(config)
, updateCalendar(updateCalendar)
, getUnixTimeStamp(getUnixTimeStamp)
, nextUpdate(0)
{}

void uICALRelay::begin() {
    // Determine gateCount
    for (this->gateCount=0; this->gateCount<100; this->gateCount++) {
        if (!this->config.gates[this->gateCount].name) {
            break;
        }
    }

    // Setup push button pin
    pinMode(this->config.pushButtonPin, INPUT);
    digitalWrite(this->config.pushButtonPin, HIGH);

    // Setup status LED pin
    pinMode(this->config.statusLedPin, OUTPUT);
    digitalWrite(this->config.statusLedPin, HIGH);

    // Setup gate pins
    for (int i=0; i<this->gateCount; i++) {
        digitalWrite(this->config.gates[i].pin, LOW);
        pinMode(this->config.gates[i].pin, OUTPUT);
    }
}

void uICALRelay::statusLed(bool state) {
    digitalWrite(this->config.statusLedPin, !state);
}

void uICALRelay::statusLedToggle() {
    uint8_t state = digitalRead(this->config.statusLedPin);
    digitalWrite(this->config.statusLedPin, !state);
}

void uICALRelay::handleRelays() {
    unsigned unixTimeStamp = getUnixTimeStamp();

    bool update = digitalRead(this->config.pushButtonPin);
    if (!update) {
        log_info("%s", "Update button pressed");
        this->nextUpdate = 0;
    }

    if (unixTimeStamp < this->nextUpdate) {
        return;
    }

    this->statusLed(true);
    this->updateCalendar(this->config.icalURL, this->config.hostFingerprint, [&](Stream& stm) {
        int a = ESP.getFreeHeap();
        Serial.println((String)"FREE A: " + a);

        this->processStream(stm);

        int b = ESP.getFreeHeap();
        Serial.println((String)"FREE B: " + b + " (" + (b-a) + ")");
    });
    this->statusLed(true);

    unsigned wait = this->updateGates(unixTimeStamp);
    this->nextUpdate = unixTimeStamp + wait;
}

void uICALRelay::processStream(Stream& stm) {
    try {
        uICAL::istream_Stream istm(stm);
        this->cal = uICAL::Calendar::load(istm, [=](const uICAL::VEvent& event){
          return this->addEvent(event);
        });
    }
    catch (uICAL::Error ex) {
        log_error("%s: %s", ex.message.c_str(), "! Failed loading calendar");
    }
}

bool uICALRelay::addEvent(const uICAL::VEvent& event) {
    for (int i=0; i<this->gateCount; i++) {
        if (event.summary.indexOf(this->config.gates[i].name) != -1) {
            return true;
        }
    }
    return false;
}

unsigned uICALRelay::updateGates(unsigned unixTimeStamp) {
    unsigned sleep = this->config.pollPeriod;

    if (!this->cal) {
        return sleep / 10;
    }

    if (unixTimeStamp == 0) {
        log_warning("%s", "No NTP fix");
        return sleep / 10;
    }
    uICAL::DateTime now(unixTimeStamp);
    uICAL::DateTime calBegin(unixTimeStamp);
    uICAL::DateTime calEnd(unixTimeStamp + this->config.pollPeriod);

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
                String relayName = this->config.gates[i].name;
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
            uint8_t state = digitalRead(this->config.gates[i].pin);

            if (relayState[i] != state) {
                digitalWrite(this->config.gates[i].pin, relayState[i]);
            }
            log_info("Relay: %s - %s", (relayState[i] == HIGH ? "ON  " : "OFF "), this->config.gates[i].name);
        }
    }
    catch (uICAL::Error ex) {
        log_error("%s", ex.message.c_str());
    }
    log_debug("Sleep: %ds", sleep);
    return sleep;
}
