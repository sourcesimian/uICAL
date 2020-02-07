#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/icalevent.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/icalline.h"
#include "uICAL/tzidmap.h"
#include "uICAL/calendarentry.h"

namespace uICAL {
    Calendar::ptr Calendar::init(std::istream& ical) {
        return Calendar::ptr(new Calendar(ical));
    }

    Calendar::Calendar(std::istream& ical)
    {
        auto vcalendar = uICAL::VComponent::parse(ical);

        auto tzidmap = TZIdMap::init(*vcalendar.get());

        // std::cout << "TZ ID Map:" << std::endl;
        tzidmap->str(std::cout);
        // std::cout << std::endl;

        auto events = vcalendar->listComponents("VEVENT");

        // std::cout << "Events: " << events.size() << std::endl;
        for (auto it = events.begin(); it != events.end(); ++it) {
            ICalEvent::ptr ev = ICalEvent::init(*it, tzidmap);
            this->events.push_back(ev);
            // std::cout << " - " << std::endl;
            // std::cout << (*it) << std::endl;

            std::cout << ev << std::endl;
            //std::cout << " - - " << std::endl;
            //std::cout << e << std::endl;
        }
        // std::cout << "---" << std::endl;
    }

    void Calendar::str(std::ostream& out) const {
        out << "CALENDAR" << std::endl;
    }

    std::ostream & operator << (std::ostream &out, const Calendar::ptr &c) {
        c->str(out);
        return out;
    }
    
    std::ostream & operator << (std::ostream &out, const Calendar &c) {
        c.str(out);
        return out;
    }

    CalendarIter::ptr CalendarIter::init(const Calendar::ptr cal, DateTime begin, DateTime end) {
        return CalendarIter::ptr(new CalendarIter(cal, begin, end));
    }

    CalendarIter::CalendarIter(const Calendar::ptr cal, DateTime begin, DateTime end)
    : cal(cal)
    {
        for (auto it = this->cal->events.begin(); it != this->cal->events.end(); ++it) {
            ICalEventIter::ptr evIt = ICalEventIter::init(*it, begin, end);

            if (evIt->next()) {  // Initialise and filter
                this->events.push_back(evIt);
            }
        }
    }

    bool CalendarIter::next() {
        if (this->events.size() == 0) {
            return false;
        }

        auto minIt = std::min_element(this->events.begin(), this->events.end());

        this->currentEntry = (*minIt)->entry();

        if (! (*minIt)->next()) {
            this->events.erase(minIt);
        }
        return true;
    }

    CalendarEntry::ptr CalendarIter::current() const {
        return this->currentEntry;
    }

}
