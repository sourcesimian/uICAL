/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_relay_h
#define uical_relay_h

#include <uICAL.h>

class uICALRelay {
    public:
        typedef struct {
            const char* name;
            const uint8_t pin;
        } Gate;

        void begin();

        void updateCalendar(Stream& stm);
        unsigned updateGates(unsigned unixTimeStamp);
        void wait(unsigned sleep);

        void statusLed(bool state);
        void statusLedToggle();

        static const char* hostFingerprint;
        static const char* icalURL;
        static const int pollPeriod;

    protected:
        static const Gate gates[];
        static const uint8_t statusLedPin;
        static const uint8_t pushButtonPin;

        bool addEvent(const uICAL::VEvent& event);

        int gateCount;
        uICAL::Calendar_ptr cal;
};

#endif
