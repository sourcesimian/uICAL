#ifndef uical_datestamp_h
#define uical_datestamp_h

#include "uICAL/datetime.h"
#include "uICAL/epochtime.h"

namespace uICAL {
    class DateStamp {
        public:
            DateStamp();
            DateStamp(const std::string datestamp);
            DateStamp(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second);

            void str(std::ostream& out) const;

            unsigned year;
            unsigned month;
            unsigned day;
            unsigned hour;
            unsigned minute;
            unsigned second;

            bool valid() const;

            DateTime::Day dayOfWeek() const;
            unsigned weekNo() const;
            unsigned dayOfYear() const;
            unsigned daysInMonth() const;
            unsigned daysInYear() const;

            void incYear(unsigned n);
            void incMonth(unsigned n);
            void incWeek(unsigned n, DateTime::Day wkst);
            void incDay(unsigned n);
            void incHour(unsigned n);
            void incMinute(unsigned n);
            void incSecond(unsigned n);

            void decDay(unsigned n);
            void decMonth(unsigned n);

            void setWeekNo(unsigned n);

            DateStamp& operator = (const DateStamp &ds);

            bool operator > (const DateStamp& ds) const;
            bool operator < (const DateStamp& ds) const;
            bool operator <= (const DateStamp& ds) const;
            bool operator == (const DateStamp& ds) const;
            bool operator != (const DateStamp& ds) const;

        protected:
            seconds_t index() const;
            void validate() const;
            DateTime::Day getWeekDay(unsigned days) const;
    };
    
    std::ostream & operator << (std::ostream &out, const DateStamp& dt);

}
#endif
