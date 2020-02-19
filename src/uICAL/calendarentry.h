/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
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
            static ptr init(Type type, const string& summary, const DateTime& start);
            static ptr init(Type type, const string& summary, const DateTime& start, const DatePeriod& span);

            CalendarEntry(Type type, const string& summary, const DateTime& start);
            CalendarEntry(Type type, const string& summary, const DateTime& start, const DatePeriod& span);
            
            Type type() const;
            const string summary() const;
            DateTime start() const;
            DateTime end() const;

            void str(std::ostream& out) const;

            static string asString(Type type);

        protected:
            Type _type;
            string _summary;
            DateTime _start;
            DatePeriod _span;
    };

    std::ostream& operator << (std::ostream& out, const CalendarEntry::ptr& e);
}

#endif