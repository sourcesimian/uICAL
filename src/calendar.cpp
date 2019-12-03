#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/calendar.h"
#include "uICAL/icalevent.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/icalline.h"
#include "uICAL/tzidmap.h"
#include "uICAL/entry.h"

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
            auto ev = ICalEvent::init(*it, tzidmap);
            this->events.push_back(ev);
            // std::cout << " - " << std::endl;
            // std::cout << (*it) << std::endl;

            // std::cout << e << std::endl;
            //std::cout << " - - " << std::endl;
            //std::cout << e << std::endl;
        }
        // std::cout << "---" << std::endl;
    }

    // bool Calendar::event_init() {
    //     this->
    // }

    bool Calendar::next_entry() {
        std::vector<events_t::iterator> complete;

        for (auto it = this->events.begin(); it != this->events.end(); ++it) {
            if (!(*it)->next()) {
                complete.push_back(it);
            }
        }
        for (auto it : complete) {
            this->events.erase(it);
        }
    }
    
    Entry Calendar::current_entry() const {
        return Entry(Entry::Type::NONE, "", DateTime());
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
