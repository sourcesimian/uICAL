/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#include "uICALRelay.h"
#include "uICALRelay_config.h"

uICALRelay::uICALRelay(getTimestamp_t getUnixTimeStamp, getUrl_t httpGet) {
    this->getUnixTimeStamp = getUnixTimeStamp;
    this->httpGet = httpGet;

    for (this->gateCount=0; this->gateCount<100; this->gateCount++) {
        if (!this->gates[this->gateCount].name) {
            break;
        } 
    }

    pinMode(this->pushButtonPin, INPUT);
    digitalWrite(this->pushButtonPin, HIGH);

    pinMode(this->statusLedPin, OUTPUT);
    digitalWrite(this->statusLedPin, LOW);

    for (int i=0; i<this->gateCount; i++) {
        digitalWrite(this->gates[i].pin, LOW);
        pinMode(this->gates[i].pin, OUTPUT);
    }
}

unsigned uICALRelay::loop() {
    this->updateCalendar();
    unsigned sleep = this->updateGates();
    return sleep;
}

void uICALRelay::updateCalendar() {
    bool led = digitalRead(this->statusLedPin);
    Serial.println("# Updating calendar");
    digitalWrite(this->statusLedPin, !led);

    String payload = this->httpGet(ICAL_URL);

    if (payload.length()) {
        uICAL::istream_String inp(payload);
        try {
            this->cal = uICAL::Calendar::init(inp);
        }
        catch (uICAL::Error ex) {
            Serial.println("! Failed loading calendar");
            Serial.println(ex.message);
        }
    }
    else {
        Serial.println((String)"! Failed fetching calendar ");
    }
    digitalWrite(this->statusLedPin, led);
}

unsigned uICALRelay::updateGates() {
  unsigned sleep = POLL_PERIOD;

  if (!this->cal) {
    return sleep / 10;
  }

  unsigned long unixTime = this->getUnixTimeStamp();
  if (unixTime == 0) {
    Serial.println("No NTP fix");
    return sleep / 10;
  }
  uICAL::DateTime now(unixTime);  
  uICAL::DateTime calBegin(unixTime);
  uICAL::DateTime calEnd(unixTime + POLL_PERIOD);

  try { 
    uICAL::CalendarIter::ptr calIt = uICAL::CalendarIter::init(this->cal, calBegin, calEnd);

    uint8_t relayState[this->gateCount];

    for (int i=0; i<this->gateCount; i++) {
      relayState[i] = LOW;
    }

    while (calIt->next()) {
      uICAL::CalendarEntry::ptr entry = calIt->current();

      uICAL::DateTime dtStart = entry->start();
      uICAL::string start = dtStart.as_str();
      String summary = entry->summary();
      summary.toLowerCase();
      Serial.println((String)"# Event: " + start + " " + entry->summary());

      for (int i=0; i<this->gateCount; i++) {
        String relayName = this->gates[i].name;
        relayName.toLowerCase();
        if (entry->start() <= now && now < entry->end()) {
          if (summary.indexOf(relayName) != -1) {
            relayState[i] = HIGH;
            sleep = std::min(sleep, (unsigned)(entry->end() - now).totalSeconds());
          }
          else if (now > entry->start()) {
            sleep = std::min(sleep, (unsigned)(entry->start() - now).totalSeconds());
          }
        }
      }
    }

    Serial.println(uICAL::string("Now: ") + now.as_str());
    for (int i=0; i<this->gateCount; i++) {
      uint8_t state = digitalRead(this->gates[i].pin);

      if (relayState[i] != state) {
        digitalWrite(this->gates[i].pin, relayState[i]);
      }
      Serial.println((String)"Relay: " + (relayState[i] == HIGH ? "ON  " : "OFF ") + this->gates[i].name);
    }
  }
  catch (uICAL::Error ex) {
    Serial.println(ex.message);
  }
  return sleep;
}

void uICALRelay::wait(unsigned sleep) {
    Serial.println("# Wait: " + String(sleep) + "s");

    sleep *= 1000;
    while (sleep > 0) {
        delay(200);
        sleep -= 200;
        bool update = digitalRead(this->pushButtonPin);
        if (!update) {
            Serial.println("Update button");
            break;
        }
    }
}
