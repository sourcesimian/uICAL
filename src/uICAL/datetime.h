/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_datetime_h
#define uical_datetime_h

#include "uICAL/base.h"
#include "uICAL/epochtime.h"

namespace uICAL {
    class DateStamp;
    class DatePeriod;
    class TZ;

    class DateTime : public Base {
        public:
            DateTime();
            DateTime(const string& datetime);
            DateTime(const string& datetime, const TZMap_ptr& tzmap);
            DateTime(const DateStamp& datestamp, const TZ_ptr& tz);
            DateTime(seconds_t epochSeconds);
            DateTime(seconds_t epochSeconds, const TZ_ptr& tz);
            DateTime(const DateTime&) = default;

            void str(ostream& out) const;

            bool valid() const;

            DateStamp datestamp() const;
            DateStamp datestamp(const TZ_ptr& tz) const;

            enum class Day {
                NONE, MON, TUE, WED, THU, FRI, SAT, SUN
            };

            static unsigned daysUntil(DateTime::Day today, DateTime::Day then);
            static unsigned daysUntil(DateTime::Day today, int index, DateTime::Day then, unsigned span);
            static DateTime::Day dayOfWeekAfter(DateTime::Day today, unsigned days);

            DateTime& operator = (const DateTime& dt);
            DatePeriod operator + (const DateTime& dt) const;
            DatePeriod operator - (const DateTime& dt) const;
            DateTime operator + (const DatePeriod& dp) const;
            DateTime operator - (const DatePeriod& dp) const;

            bool operator > (const DateTime& dt) const;
            bool operator < (const DateTime& dt) const;
            bool operator <= (const DateTime& dt) const;
            bool operator == (const DateTime& dt) const;

            TZ_ptr tz;

        protected:
            void construct(const string& datetime, const TZMap_ptr& tzmap);
            void construct(const DateStamp& ds, const TZ_ptr& tz);
            void assert_awareness(const DateTime& other) const;

            EpochTime epochtime;
    };

    ostream& operator << (ostream& out, const DateTime::Day& day);
}
#endif
