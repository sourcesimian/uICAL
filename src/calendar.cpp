#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/event.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/icalline.h"
#include "uICAL/timezone.h"

namespace uICAL {
    Calendar::ptr Calendar::init(const std::string ical) {
        return Calendar::ptr(new Calendar(ical));
    }

    Calendar::Calendar(const std::string ical)
    {
        auto vcalendar = uICAL::VComponent::parse(ical);

        //std::cout << *(vcalendar.get()) << std::endl;
        //std::cout << "---" << std::endl;

        auto events = vcalendar->listComponents("VEVENT");

        auto tz = Timezones(*vcalendar.get());

        std::cout << "Events: " << events.size() << std::endl;
        for (auto it = events.begin(); it != events.end(); ++it) {
            std::cout << " - " << std::endl;
            std::cout << (*it) << std::endl;

            auto e = Event(*(it->get()), tz);
            std::cout << e << std::endl;
            //std::cout << " - - " << std::endl;
            //std::cout << e << std::endl;


        }
        std::cout << "---" << std::endl;
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
}
