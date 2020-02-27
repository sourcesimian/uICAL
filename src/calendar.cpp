/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/vevent.h"
#include "uICAL/vcomponent.h"
#include "uICAL/vcomponentstream.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"
#include "uICAL/tzmap.h"
#include "uICAL/calendarentry.h"
#include "uICAL/logging.h"

namespace uICAL {
    Calendar::ptr Calendar::init(istream& ical) {
        return Calendar::ptr(new Calendar(ical));
    }

    Calendar::Calendar(istream& ical)
    {
        VLineStream lines(ical);
        VComponentStream components(lines);

        VComponent::ptr vcalendar = components.next();

        this->tzmap = TZMap::init(*vcalendar.get());

        auto events = vcalendar->listComponents("VEVENT");

        for (auto comp : events) {
            VEvent::ptr ev = VEvent::init(comp, this->tzmap);
            this->events.push_back(ev);
            log_trace("Found %s", ev->as_str().c_str());
        }
    }

    void Calendar::str(ostream& out) const {
        out << "CALENDAR" << uICAL::endl;
    }

    CalendarIter::ptr CalendarIter::init(const Calendar::ptr cal, const DateTime& begin, const DateTime& end) {
        return CalendarIter::ptr(new CalendarIter(cal, begin, end));
    }

    CalendarIter::CalendarIter(const Calendar::ptr cal, const DateTime& begin, const DateTime& end)
    : cal(cal)
    {
        if (begin.valid() && end.valid() && end < begin) {
            log_error("Begin and end describe a negative range: %s -> %s", begin.as_str().c_str(), end.as_str().c_str());
            throw ValueError("Begin and end describe a negative range");
        }

        for (auto ev : this->cal->events) {
            VEventIter::ptr evIt = VEventIter::init(ev, begin, end);

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
            log_warning("%s", "No more entries");
            throw RecurrenceError("No more entries");
        }
        return this->currentEntry;
    }
}
