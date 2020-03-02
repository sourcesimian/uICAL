/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_calendarentry_h
#define uical_calendarentry_h

#include "uICAL/base.h"
#include "uICAL/datetime.h"
#include "uICAL/dateperiod.h"

namespace uICAL {
    class CalendarEntry : public Base {
        public:
            enum class Type {
                NONE, EVENT,
            };

            CalendarEntry(Type type, const string& summary, const DateTime& start);
            CalendarEntry(Type type, const string& summary, const DateTime& start, const DatePeriod& span);

            Type type() const;
            const string summary() const;
            DateTime start() const;
            DateTime end() const;

            void str(ostream& out) const;

            static string asString(Type type);

        protected:
            Type _type;
            string _summary;
            DateTime _start;
            DatePeriod _span;
    };
}

#endif
