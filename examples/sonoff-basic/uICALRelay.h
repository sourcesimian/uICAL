/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_relay_h
#define uical_relay_h

#include <uICAL.h>

class uICALRelay {
    public:
        using updateCalendar_t = std::function<void (String& url,
                                                     String& hostFingerprint,
                                                     std::function<void (Stream&)> processStream)>;
        using getUnixTimeStamp_t = std::function<unsigned ()>;
        using setGate_t = std::function<void (const char* id, bool state)>;

        uICALRelay(updateCalendar_t updateCalendar, getUnixTimeStamp_t getUnixTimeStamp, setGate_t setGate);

        void config(String icalURL, int pollPeriod, String hostFingerprint = emptyString);
        void configGate(const char* id, String name);

        void handleRelays();

        void forceUpdate();

    protected:
        void processStream(Stream& stm);
        unsigned updateGates(unsigned unixTimeStamp);
        bool addEvent(const uICAL::VEvent& event);

        updateCalendar_t updateCalendar;
        getUnixTimeStamp_t getUnixTimeStamp;
        setGate_t setGate;

        String icalURL;
        int pollPeriod;
        String hostFingerprint;
        std::map<String, const char*> gates;

        uICAL::Calendar_ptr cal;
        unsigned nextUpdate;
        unsigned long lastMillis;
};

#endif
