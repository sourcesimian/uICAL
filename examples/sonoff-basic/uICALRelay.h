/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_relay_h
#define uical_relay_h

#include <uICAL.h>

class uICALRelay {
    public:
        using updateCalendar_t = std::function<void (const char* url,
                                                     const char* hostFingerprint,
                                                     std::function<void (Stream&)> processStream)>;
        using getUnixTimeStamp_t = std::function<unsigned ()>;

        struct gate_t {
            const char* name;
            const uint8_t pin;
        };

        struct config_t {
            const char* icalURL;
            const char* hostFingerprint;
            const int pollPeriod;

            const uint8_t statusLedPin;
            const uint8_t pushButtonPin;

            const gate_t* gates;
        };

        uICALRelay(config_t& config, updateCalendar_t updateCalendar, getUnixTimeStamp_t getUnixTimeStamp);

        void begin();
        void handleRelays();

        void statusLed(bool state);
        void statusLedToggle();

    protected:
        void processStream(Stream& stm);
        unsigned updateGates(unsigned unixTimeStamp);
        bool addEvent(const uICAL::VEvent& event);

        config_t& config;
        updateCalendar_t updateCalendar;
        getUnixTimeStamp_t getUnixTimeStamp;

        int gateCount;
        uICAL::Calendar_ptr cal;
        unsigned nextUpdate;
};

#endif
