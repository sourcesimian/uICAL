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
, nextGateUpdate((unsigned)-1)
, nextCalendarUpdate(0)
, lastMillis(0)
{}

void uICALRelay::config(String icalURL, int pollCalendarPeriod, String hostFingerprint) {
    this->nextCalendarUpdate = 0;

    log_info("Configure calendar URL: \"%s\"", icalURL.c_str());
    this->icalURL = icalURL;

    log_info("Configure calendar Poll Period: %d", pollCalendarPeriod);
    this->pollCalendarPeriod = pollCalendarPeriod;

    log_info("Configure calendar Host Fingerprint: \"%s\"", hostFingerprint.c_str());
    this->hostFingerprint = hostFingerprint;

    this->gates.clear();
}

void uICALRelay::configGate(const char* id, String name) {
    this->gates[name] = id;
    log_info("Configure gate '%s': \"%s\"", id, name.c_str());
}

void uICALRelay::forceUpdate() {
    this->nextCalendarUpdate = 0;
}

void uICALRelay::handleRelays() {
    unsigned unixTimeStamp;

    if ((this->lastMillis + 1000) > millis()) {
        return;
    }

    unixTimeStamp = getUnixTimeStamp();
    if (unixTimeStamp == 0) {
        log_warning("%s", "No NTP fix");
        return;
    }

    if (unixTimeStamp >= this->nextGateUpdate) {
        this->nextGateUpdate = this->updateGates(unixTimeStamp);
    }

    if (unixTimeStamp >= this->nextCalendarUpdate) {
        this->updateCalendar(this->icalURL, this->hostFingerprint, [&](Stream& stm) {
            if(this->processStream(stm) == true) {
                this->nextGateUpdate = 0;
            }
        });
        this->nextCalendarUpdate = unixTimeStamp + this->pollCalendarPeriod;
    }

    if (unixTimeStamp >= this->nextGateUpdate) {
        unixTimeStamp = getUnixTimeStamp();
        this->nextGateUpdate = this->updateGates(unixTimeStamp);
    }

    this->lastMillis = millis();
}

bool uICALRelay::processStream(Stream& stm) {
    try {
        uICAL::istream_Stream istm(stm);
        this->cal = uICAL::Calendar::load(istm, [=](const uICAL::VEvent& event){
            return this->useEvent(event);
        });
        return true;
    }
    catch (uICAL::Error ex) {
        log_error("%s: %s", ex.message.c_str(), "! Failed loading calendar");
        return false;
    }
}

bool uICALRelay::useEvent(const uICAL::VEvent& event) {
    for (auto it = this->gates.begin(); it != this->gates.end(); ++it) {
        if (event.summary.startsWith(it->first)) {
            return true;
        }
    }
    return false;
}

unsigned uICALRelay::updateGates(unsigned unixTimeStamp) {
    unsigned wait = this->gateUpdateWindow;

    uICAL::DateTime now(unixTimeStamp);
    uICAL::DateTime calBegin(unixTimeStamp);
    uICAL::DateTime calEnd(unixTimeStamp + this->gateUpdateWindow);

    log_info("Current Time: %s", now.as_str().c_str());
    log_info("calBegin Time: %s", calBegin.as_str().c_str());
    log_info("calEnd Time: %s", calEnd.as_str().c_str());

    auto updateWait = [&](uICAL::DateTime dt) {
        wait = std::min(wait, (unsigned)(dt - now).totalSeconds());
    };
    
    std::map<const char*, bool> gateStates;
    for (auto it = this->gates.begin(); it != this->gates.end(); ++it) {
        gateStates[it->second] = false;
    }

    try {
        if (this->cal) {
            uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(this->cal, calBegin, calEnd);

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
                            updateWait(entry->end());
                        }
                    }
                    else if (now < entry->start()) {
                        updateWait(entry->start());
                    }
                }
            }
        }
    }
    catch (uICAL::Error ex) {
        log_error("%s", ex.message.c_str());
    }

    for (auto it = gateStates.begin(); it != gateStates.end(); ++it) {
        log_info("Gate: '%s' %s", it->first, (it->second ? "ON  " : "OFF "));
        this->setGate(it->first, it->second);
    }

    log_debug("Wait: %ds", wait);
    return unixTimeStamp + wait;
}
