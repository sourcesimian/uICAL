/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_calendariter_h
#define uical_calendariter_h

namespace uICAL {
    class CalendarIter {
        public:
            CalendarIter(const Calendar_ptr cal, const DateTime& begin, const DateTime& end);

            bool next();
            CalendarEntry_ptr current() const;

        protected:
            const Calendar_ptr cal;

            using events_t = std::vector<VEventIter_ptr>;
            events_t events;

            CalendarEntry_ptr currentEntry;
    };
}
#endif
