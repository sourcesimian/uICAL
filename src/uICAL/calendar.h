/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_calendar_h
#define uical_calendar_h

#include "uICAL/base.h"
#include "uICAL/vevent.h"

namespace uICAL {
    class Calendar : public Base {
        public:
            using ptr = std::shared_ptr<Calendar>;
            static ptr init(istream& ical);
            Calendar(istream& ical);

            void str(ostream& out) const;

            TZMap::ptr tzmap;

            friend class CalendarIter;
        protected:
            using events_t = std::vector<VEvent::ptr>;
            events_t events;
    };

    class CalendarIter {
        public:
            using ptr = std::shared_ptr<CalendarIter>;
            static ptr init(const Calendar::ptr cal, const DateTime& begin, const DateTime& end);
            CalendarIter(const Calendar::ptr cal, const DateTime& begin, const DateTime& end);

            bool next();
            CalendarEntry::ptr current() const;

        protected:
            const Calendar::ptr cal;

            using events_t = std::vector<VEventIter::ptr>;
            events_t events;

            CalendarEntry::ptr currentEntry;
    };
}
#endif
