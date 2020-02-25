/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_calendar_h
#define uical_calendar_h

#include "uICAL/base.h"
#include "uICAL/icalevent.h"

namespace uICAL {
    class Calendar : public Base {
        public:
            using ptr = std::shared_ptr<Calendar>;
            static ptr init(istream& ical);
            Calendar(istream& ical);

            bool valid() const;
            void str(ostream& out) const;

            TZMap::ptr tzmap;

            friend class CalendarIter;
        protected:
            using events_t = std::vector<ICalEvent::ptr>;
            events_t events;
            bool _valid;
    };

    ostream& operator << (ostream& out, const Calendar::ptr& c);
    ostream& operator << (ostream& out, const Calendar& c);

    class CalendarIter {
        public:
            using ptr = std::shared_ptr<CalendarIter>;
            static ptr init(const Calendar::ptr cal, const DateTime& begin, const DateTime& end);
            CalendarIter(const Calendar::ptr cal, const DateTime& begin, const DateTime& end);

            bool next();
            CalendarEntry::ptr current() const;

        protected:
            const Calendar::ptr cal;

            using events_t = std::vector<ICalEventIter::ptr>;
            events_t events;

            CalendarEntry::ptr currentEntry;
    };
}
#endif
