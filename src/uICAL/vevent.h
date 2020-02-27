/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_icalevent_h
#define uical_icalevent_h

#include "uICAL/base.h"
#include "uICAL/tzmap.h"
#include "uICAL/rruleiter.h"
#include "uICAL/vcomponent.h"
#include "uICAL/calendarentry.h"


namespace uICAL {
    class VComponent;
    class DateTime;

    class VEvent : public VComponent {
        public:
            using ptr = std::shared_ptr<VEvent>;
            static ptr init(const VComponent::ptr& event, const TZMap::ptr& tzmap);
            VEvent(const VComponent::ptr& event, const TZMap::ptr& tzmap);

            void str(ostream& out) const;

            friend class VEventIter;
        private:
            DateTime start;
            DateTime end;
            string summary;
            RRule::ptr rrule;
    };

    class VEventIter {
        public:
            using ptr = std::shared_ptr<VEventIter>;
            static ptr init(const VEvent::ptr ice, DateTime begin, DateTime end);
            VEventIter(const VEvent::ptr ice, DateTime begin, DateTime end);

            bool next();
            DateTime now() const;
            CalendarEntry::ptr entry() const;

            friend bool operator < (const VEventIter::ptr& a, const VEventIter::ptr& b);
        private:
            const VEvent::ptr ice;
            RRuleIter::ptr rrule;
            DateTime range_begin;
    };

    bool operator < (const VEventIter::ptr& a, const VEventIter::ptr& b);
}
#endif
