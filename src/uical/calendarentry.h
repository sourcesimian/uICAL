#ifndef uical_calendarentry_h
#define uical_calendarentry_h

#include "uICAL/datetime.h"
#include "uICAL/dateperiod.h"

namespace uICAL {
    class CalendarEntry {
        public:
            enum class Type {
                NONE, EVENT,
            };

            using ptr = std::shared_ptr<CalendarEntry>;
            static ptr init(Type type, std::string summary, DateTime start);
            static ptr init(Type type, std::string summary, DateTime start, DatePeriod span);

            CalendarEntry(Type type, std::string summary, DateTime start);
            CalendarEntry(Type type, std::string summary, DateTime start, DatePeriod span);
            
            Type type() const;
            const std::string summary() const;
            DateTime start() const;
            DatePeriod duration() const;

            void str(std::ostream& out) const;

            static std::string asString(Type type);

        protected:
            Type _type;
            std::string _summary;
            DateTime _start;
            DatePeriod _span;
    };

    std::ostream& operator << (std::ostream& out, const CalendarEntry::ptr& e);
}

#endif