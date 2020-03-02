/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_calendar_h
#define uical_calendar_h

#include "uICAL/base.h"

namespace uICAL {
    class Calendar : public Base {
        public:
            Calendar();

            static Calendar_ptr load(istream& ical);
            static Calendar_ptr load(istream& ical, TZMap_ptr& tzmap);

            void addEvent(const VEvent_ptr &);

            void str(ostream& out) const;

            friend class CalendarIter;
        protected:
            using events_t = std::vector<VEvent_ptr>;
            events_t events;
    };
}
#endif
