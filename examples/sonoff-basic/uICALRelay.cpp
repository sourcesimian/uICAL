/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#include "uICALRelay.h"
#include "uICAL/logging.h"


uICALRelay::uICALRelay(updateCalendar_t updateCalendar, getUnixTimeStamp_t getUnixTimeStamp, setGate_t setGate)
: updateCalendar(updateCalendar)
, getUnixTimeStamp(getUnixTimeStamp)
, setGate(setGate)
, icalURL(emptyString)
, nextUpdate(0)
, lastMillis(0)
{}

void uICALRelay::config(String icalURL, int pollPeriod, String hostFingerprint) {
    this->nextUpdate = 0;

    log_info("Config URL: \"%s\"", icalURL.c_str());
    this->icalURL = icalURL;

    log_info("Config Poll Period: %d", pollPeriod);
    this->pollPeriod = pollPeriod;

    log_info("Config Host Fingerprint: \"%s\"", hostFingerprint.c_str());
    this->hostFingerprint = hostFingerprint;

    this->gates.clear();
}

void uICALRelay::configGate(const char* id, String name) {
    this->gates[name] = id;
    log_info("Config gate '%s': \"%s\"", id, name.c_str());
}

void uICALRelay::forceUpdate() {
    this->nextUpdate = 0;
}

void uICALRelay::handleRelays() {
    if ((this->lastMillis + 1000) > millis()) {
        return;
    }

    unsigned unixTimeStamp = getUnixTimeStamp();
    if (unixTimeStamp < this->nextUpdate) {
        return;
    }

    this->updateCalendar(this->icalURL, this->hostFingerprint, [&](Stream& stm) {
        this->processStream(stm);
    });

    unixTimeStamp = getUnixTimeStamp();
    unsigned wait = this->updateGates(unixTimeStamp);
    this->nextUpdate = unixTimeStamp + wait;
    this->lastMillis = millis();
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
    for (auto it = this->gates.begin(); it != this->gates.end(); ++it) {
        if (event.summary.startsWith(it->first)) {
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

        std::map<const char*, bool> gateStates;
        for (auto it = this->gates.begin(); it != this->gates.end(); ++it) {
            gateStates[it->second] = false;
        }
        
        while (calIt->next()) {
            uICAL::CalendarEntry_ptr entry = calIt->current();
            log_debug("Event @ %s -> %s : %s",
                      entry->start().as_str().c_str(),
                      entry->end().as_str().c_str(),
                      entry->summary().c_str());

            for (auto it = this->gates.begin(); it != this->gates.end(); ++it) {
                if (entry->start() <= now && now < entry->end()) {

                    if (entry->summary() == it->first) {
                        gateStates[it->second] = true;
                        sleep = std::min(sleep, (unsigned)(entry->end() - now).totalSeconds());
                    }
                }
                else if (now < entry->start()) {
                    sleep = std::min(sleep, (unsigned)(entry->start() - now).totalSeconds());
                }
            }
        }

        for (auto it = gateStates.begin(); it != gateStates.end(); ++it) {
            log_info("Gate: '%s' %s", it->first, (it->second ? "ON  " : "OFF "));
            setGate(it->first, it->second);
        }
    }
    catch (uICAL::Error ex) {
        log_error("%s", ex.message.c_str());
    }
    log_debug("Sleep: %ds", sleep);
    return sleep;
}
