/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/icalevent.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/icalline.h"
#include "uICAL/tzmap.h"
#include "uICAL/calendarentry.h"
#include "uICAL/debug.h"

namespace uICAL {
    Calendar::ptr Calendar::init(std::istream& ical) {
        return Calendar::ptr(new Calendar(ical));
    }

    Calendar::Calendar(std::istream& ical)
    {
        auto vcalendar = uICAL::VComponent::parse(ical);

        this->tzmap = TZMap::init(*vcalendar.get());

        auto events = vcalendar->listComponents("VEVENT");

        for (auto comp : events) {
            ICalEvent::ptr ev = ICalEvent::init(comp, this->tzmap);
            this->events.push_back(ev);
            debug(string("Calendar ") + ev->str());
        }
    }

    void Calendar::str(std::ostream& out) const {
        out << "CALENDAR" << std::endl;
    }

    std::ostream& operator << (std::ostream& out, const Calendar::ptr& c) {
        c->str(out);
        return out;
    }
    
    std::ostream& operator << (std::ostream& out, const Calendar& c) {
        c.str(out);
        return out;
    }

    CalendarIter::ptr CalendarIter::init(const Calendar::ptr cal, const DateTime& begin, const DateTime& end) {
        return CalendarIter::ptr(new CalendarIter(cal, begin, end));
    }

    CalendarIter::CalendarIter(const Calendar::ptr cal, const DateTime& begin, const DateTime& end)
    : cal(cal)
    {
        if (begin.valid() && end.valid() && end < begin) {
            throw ValueError("Begin and end describe a negative range");
        }

        for (auto ev : this->cal->events) {
            ICalEventIter::ptr evIt = ICalEventIter::init(ev, begin, end);

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
        if (! this->currentEntry) {
            throw RecurrenceError("No more entries");
        }
        return this->currentEntry;
    }
}
