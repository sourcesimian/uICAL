#ifndef uical_event_h
#define uical_event_h

#include "uICAL/datetime.h"
#include "uICAL/tzidmap.h"
#include "uICAL/rruleparser.h"

namespace uICAL {
    class VComponent;

    class Event {
        public:
            using ptr = std::shared_ptr<Event>;

            static ptr init(VComponent& event, const TZIdMap::ptr& tzidmap);

            Event(VComponent& event, const TZIdMap::ptr& tzidmap);
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
