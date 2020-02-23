/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_relay_h
#define uical_relay_h

#include <uICAL.h>

class uICALRelay {
    public:
        using getTimestamp_t = std::function<unsigned ()>;
        using getUrl_t = std::function<String (const char*)>;

        typedef struct {
            const char* name;
            const uint8_t pin;
        } Gate;

        uICALRelay(getTimestamp_t getUnixTimeStamp,
                   getUrl_t httpGet);

        unsigned loop();
        void updateCalendar();
        unsigned updateGates();
        void wait(unsigned sleep);

    protected:
        static const Gate gates[];
        static const uint8_t statusLedPin;
        static const uint8_t pushButtonPin;
        int gateCount;
        getTimestamp_t getUnixTimeStamp;
        getUrl_t httpGet;
        uICAL::Calendar::ptr cal;
};

#endif
