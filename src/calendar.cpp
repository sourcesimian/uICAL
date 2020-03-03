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
#include "uICAL/tzmap.h"
#include "uICAL/vevent.h"
#include "uICAL/vobject.h"
#include "uICAL/vobjectstream.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"

namespace uICAL {
    Calendar_ptr Calendar::load(istream& ical) {
        TZMap_ptr tzmap = new_ptr<TZMap>();
        return Calendar::load(ical, tzmap, [](const VEvent&) { return true; });
    }

    Calendar_ptr Calendar::load(istream& ical, eventP_t addEvent) {
        TZMap_ptr tzmap = new_ptr<TZMap>();
        return Calendar::load(ical, tzmap, addEvent);
    }

    Calendar_ptr Calendar::load(istream& ical, TZMap_ptr& tzmap) {
        return Calendar::load(ical, tzmap, [](const VEvent&) { return true; });
    }

    Calendar_ptr Calendar::load(istream& ical, TZMap_ptr& tzmap, eventP_t addEvent) {
        VLineStream lines(ical);
        VObject_ptr obj = new_ptr<VObject>();
        VObjectStream stm(lines, obj);

        if (obj->getName() != "VCALENDAR") {
            log_error("Parse error: Did not expect: %s", obj->getName().c_str());
            throw ParseError(string("Parse error: Did not expect: ") + obj->getName().c_str());
        }

        Calendar_ptr cal = new_ptr<Calendar>();

        for (;;) {
            string name = stm.nextChild();
            if (name.empty()) {
                break;
            }

            if (name == "VTIMEZONE") {
                VObject_ptr child = stm.loadChild();
                tzmap->add(child);
            }
            else if (name == "VEVENT") {
                VObject_ptr child = stm.loadChild();
                VEvent_ptr event = new_ptr<VEvent>(child, tzmap);
                if (addEvent(*event)) {
                    cal->addEvent(event);
                } else {
                    log_trace("Event skipped: %s @ %s", event->summary.c_str(), event->start.as_str().c_str());
                }
            }
            else {
                stm.skipChild();
                log_trace("VObject skipped: %s", name.c_str());
            }
        }
        return cal;
    }

    Calendar::Calendar() {}

    void Calendar::addEvent(const VEvent_ptr& event) {
        this->events.push_back(event);
    }

    void Calendar::str(ostream& out) const {
        out << "CALENDAR" << uICAL::endl;
    }
}
