#ifndef uical_event_h
#define uical_event_h

#include "uICAL/datetime.h"
#include "uICAL/timezone.h"
#include "uICAL/rruleparser.h"

namespace uICAL {
    class VComponent;

    class Event {
        public:
            using ptr = std::shared_ptr<Event>;
            Event(VComponent& event, Timezones& timezones);
            void str(std::ostream &) const;
            std::string str() const;

        private:
            DateTime start;
            DateTime end;
            std::string summary;
            RRuleParser::ptr recurrence;
    };

    std::ostream & operator << (std::ostream &out, const Event::ptr &e);
    std::ostream & operator << (std::ostream &out, const Event &e);
}
#endif
