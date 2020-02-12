#ifndef uical_icalevent_h
#define uical_icalevent_h

#include "uICAL/datetime.h"
#include "uICAL/tzmap.h"
#include "uICAL/rruleiter.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/calendarentry.h"


namespace uICAL {
    class VComponent;
    //class ICalEventIter;

    class ICalEvent {
        public:
            using ptr = std::shared_ptr<ICalEvent>;
            static ptr init(VComponent::ptr& event, const TZMap::ptr& tzmap);
            ICalEvent(VComponent::ptr& event, const TZMap::ptr& tzmap);

            std::string str() const;
            void str(std::ostream& out) const;

            friend class ICalEventIter;
        private:
            DateTime start;
            DateTime end;
            std::string summary;
            RRule::ptr rrule;
    };

    std::ostream& operator << (std::ostream& out, const ICalEvent::ptr& e);
    std::ostream& operator << (std::ostream& out, const ICalEvent& e);

    class ICalEventIter {
        public:
            using ptr = std::shared_ptr<ICalEventIter>;
            static ptr init(const ICalEvent::ptr ice, DateTime begin, DateTime end);
            ICalEventIter(const ICalEvent::ptr ice, DateTime begin, DateTime end);

            bool next();
            DateTime now() const;
            CalendarEntry::ptr entry() const;

            friend bool operator < (const ICalEventIter::ptr& a, const ICalEventIter::ptr& b);
        private:
            const ICalEvent::ptr ice;
            RRuleIter::ptr rrule;
    };

    bool operator < (const ICalEventIter::ptr& a, const ICalEventIter::ptr& b);
}
#endif
