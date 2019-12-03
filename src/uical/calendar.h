#ifndef uical_calendar_h
#define uical_calendar_h

#include "uICAL/icalevent.h"

namespace uICAL {
    class Calendar {
        public:
            using ptr = std::shared_ptr<Calendar>;
            static ptr init(std::istream& ical);
            Calendar(std::istream& ical);

            bool next_entry();
            Entry current_entry() const;
            
            void str(std::ostream& out) const;

        protected:
            using events_t = std::vector<ICalEvent::ptr>;
            events_t events;
    };

    std::ostream & operator << (std::ostream &out, const Calendar::ptr &c);
    std::ostream & operator << (std::ostream &out, const Calendar &c);
}
#endif
