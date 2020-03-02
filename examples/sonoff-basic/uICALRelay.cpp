/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <Arduino.h>

#include "uICALRelay.h"
#include "uICALRelay_config.h"

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
        uICAL::Calendar_ptr cal = this->cal = uICAL::Calendar::load(istm);

        if (cal->valid()) {
            this->cal = cal;
        }
    }
    catch (uICAL::Error ex) {
        Serial.println("! Failed loading calendar");
        Serial.println(ex.message);
    }
}

unsigned uICALRelay::updateGates(unsigned unixTimeStamp) {
  unsigned sleep = this->pollPeriod;

  if (!this->cal) {
    return sleep / 10;
  }

  if (unixTimeStamp == 0) {
    Serial.println("No NTP fix");
    return sleep / 10;
  }
  uICAL::DateTime now(unixTimeStamp);
  uICAL::DateTime calBegin(unixTimeStamp);
  uICAL::DateTime calEnd(unixTimeStamp + this->pollPeriod);

  try {
    uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(this->cal, calBegin, calEnd);

    uint8_t relayState[this->gateCount];

    for (int i=0; i<this->gateCount; i++) {
      relayState[i] = LOW;
    }

    while (calIt->next()) {
      uICAL::CalendarEntry_ptr entry = calIt->current();

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
