/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_veventiter_h
#define uical_veventiter_h

namespace uICAL {
    class VEvent;
    class VComponent;
    class VObjectStream;
    class DateTime;
    class RRuleIter;

    class VEventIter {
        public:
            VEventIter(const VEvent_ptr ice, DateTime begin, DateTime end);

            bool next();
            DateTime now() const;
            CalendarEntry_ptr entry() const;

            friend bool operator < (const VEventIter_ptr& a, const VEventIter_ptr& b);
        private:
            const VEvent_ptr ice;
            RRuleIter_ptr rrule;
            DateTime range_begin;
    };

    bool operator < (const VEventIter_ptr& a, const VEventIter_ptr& b);
}
#endif
