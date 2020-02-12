#ifndef uical_datetime_h
#define uical_datetime_h

#include "uICAL/types.h"
#include "uICAL/epochtime.h"
#include "uICAL/tz.h"

namespace uICAL {
    class DateStamp;
    class DatePeriod;
    
    class DateTime {
        public:
            using ptr = std::shared_ptr<DateTime>;

            DateTime();
            DateTime(const std::string datetime);
            DateTime(const std::string datetime, const TZMap::ptr& tzmap);
            DateTime(DateStamp datestamp, const TZ::ptr& tz);
            DateTime(seconds_t epochSeconds);
            DateTime(seconds_t epochSeconds, const TZ::ptr& tz);

            void str(std::ostream& out) const;
            std::string str() const;

            bool valid() const;
            
            DateStamp datestamp() const;
            DateStamp datestamp(const TZ::ptr& tz) const;

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

            TZ::ptr tz;

        protected:
            void construct(const std::string datetime, const TZMap::ptr& tzmap);
            void construct(DateStamp ds, const TZ::ptr& tz);
            void assert_awareness(const DateTime& other) const;

            EpochTime epochtime;
    };

    std::ostream& operator << (std::ostream& out, const DateTime& dt);
    std::ostream& operator << (std::ostream& out, const DateTime::Day& day);
}
#endif
