/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/calendar.h"
#include "uICAL/calendarentry.h"
#include "uICAL/calendariter.h"
#include "uICAL/tzmap.h"
#include "uICAL/vevent.h"
#include "uICAL/veventiter.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"
#include "uICAL/vobject.h"
#include "uICAL/vobjectstream.h"

namespace uICAL {
    CalendarIter::CalendarIter(const Calendar_ptr cal, const DateTime& begin, const DateTime& end)
    : cal(cal)
    {
        if (begin.valid() && end.valid() && end < begin) {
            log_error("Begin and end describe a negative range: %s -> %s", begin.as_str().c_str(), end.as_str().c_str());
            throw ValueError("Begin and end describe a negative range");
        }

        for (auto ev : this->cal->events) {
            VEventIter_ptr evIt = new_ptr<VEventIter>(ev, begin, end);

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

    CalendarEntry_ptr CalendarIter::current() const {
        if (! this->currentEntry) {
            log_warning("%s", "No more entries");
            throw RecurrenceError("No more entries");
        }
        return this->currentEntry;
    }
}
