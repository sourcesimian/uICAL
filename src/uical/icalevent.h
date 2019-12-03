#ifndef uical_icalevent_h
#define uical_icalevent_h

#include "uICAL/datetime.h"
#include "uICAL/tzidmap.h"
#include "uICAL/rrule.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/entry.h"


namespace uICAL {
    class VComponent;

    class ICalEvent {
        public:
            using ptr = std::shared_ptr<ICalEvent>;
            static ptr init(VComponent::ptr& event, const TZIdMap::ptr& tzidmap);
            ICalEvent(VComponent::ptr& event, const TZIdMap::ptr& tzidmap);

            bool next();
            DateTime now() const;
            Entry entry() const;

            void str(std::ostream &) const;

        private:
            DateTime start;
            DateTime end;
            std::string summary;
            RRule::ptr rrule;
    };

    std::ostream & operator << (std::ostream &out, const ICalEvent::ptr &e);
    std::ostream & operator << (std::ostream &out, const ICalEvent &e);
}
#endif
