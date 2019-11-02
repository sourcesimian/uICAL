#ifndef uical_datetime_h
#define uical_datetime_h

#include "uICAL/epochtime.h"
#include "uICAL/tz.h"

namespace uICAL {
    class DateStamp;
    
    class DateTime {
        public:
            using ptr = std::shared_ptr<DateTime>;

            DateTime();
            DateTime(const std::string datetime);
            DateTime(DateStamp datestamp, TZ::ptr tz);

            void str(std::ostream& out) const;
            std::string str() const;

            bool valid() const;
            
            DateStamp datestamp() const;
            DateStamp datestamp(const TZ::ptr tz) const;

            enum class Day {
                NONE, MON, TUE, WED, THU, FRI, SAT, SUN
            };

            static unsigned daysUntil(DateTime::Day today, DateTime::Day then);
            static unsigned daysUntil(DateTime::Day today, int index, DateTime::Day then, unsigned span);
            static DateTime::Day dayOfWeekAfter(DateTime::Day today, unsigned days);

            DateTime& operator = (const DateTime &ds);

            bool operator > (const DateTime& dt) const;
            bool operator < (const DateTime& dt) const;
            bool operator <= (const DateTime& dt) const;
            bool operator == (const DateTime& dt) const;

            TZ::ptr tz;

        protected:
            EpochTime epochtime;
    };

    std::ostream & operator << (std::ostream &out, const DateTime& dt);
    std::ostream & operator << (std::ostream &out, const DateTime::Day &day);
}
#endif
