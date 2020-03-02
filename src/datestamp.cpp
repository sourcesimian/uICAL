/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/datecalc.h"
#include "uICAL/datestamp.h"
#include "uICAL/tz.h"

namespace uICAL {
    DateStamp::DateStamp() {
        this->year = 0;
        this->month = 0;
        this->day = 0;
        this->hour = 0;
        this->minute = 0;
        this->second = 0;
    }

    DateStamp::DateStamp(const string& datestamp) {
        for (;;) {
            try {
                if (datestamp.length() != 15) {
                    break;
                }
                this->year = datestamp.substr(0, 4).as_int();
                this->month = datestamp.substr(4, 2).as_int();
                this->day = datestamp.substr(6, 2).as_int();
                if (datestamp.substr(8, 1) != "T") {
                    break;
                }
                this->hour = datestamp.substr(9, 2).as_int();
                this->minute = datestamp.substr(11, 2).as_int();
                this->second = datestamp.substr(13, 2).as_int();
                this->validate();
                return;
            }
            catch (std::invalid_argument const &e)
            {}
            catch (std::out_of_range const &e)
            {}
        }
        throw ValueError(string("Bad datestamp: \"") + datestamp + "\"");
    }

    DateStamp::DateStamp(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second) {
        this->year = year;
        this->month = month;
        this->day = day;
        this->hour = hour;
        this->minute = minute;
        this->second = second;
        this->validate();
    }

    void DateStamp::validate() const {
        ostream m;
        for (;;) {
            m << "Invalid ";
            if (this->year < 1970) {
                m << "year: " << year; break;
            }
            if (this->month < 1 || this->month > 12) {
                m << "month: " << month; break;
            }
            if (this->day < 1 || this->day > 31) {
                m << "day: " << day; break;
            }
            if (this->hour > 23) {
                m << "hour: " << hour; break;
            }
            if (this->minute > 59) {
                m << "minute: " << minute; break;
            }
            if (this->second > 59) {
                m << "second: " << second; break;
            }
            return;
        }
        throw ValueError(m);
    }

    bool DateStamp::valid() const {
        return (this->year + this->month + this->day + this->hour + this->minute + this->second);
    }

    DateStamp& DateStamp::operator= (const DateStamp& ds) {
        this->year = ds.year;
        this->month = ds.month;
        this->day = ds.day;
        this->hour = ds.hour;
        this->minute = ds.minute;
        this->second = ds.second;
        return *this;
    }

    bool DateStamp::operator > (const DateStamp& ds) const {
        return this->index() > ds.index();
    }

    bool DateStamp::operator < (const DateStamp& ds) const {
        return this->index() < ds.index();
    }

    bool DateStamp::operator <= (const DateStamp& ds) const {
        return this->index() <= ds.index();
    }

    bool DateStamp::operator == (const DateStamp& ds) const {
        return this->index() == ds.index();
    }

    bool DateStamp::operator != (const DateStamp& ds) const {
        return this->index() != ds.index();
    }

    seconds_t DateStamp::index() const {
        return EpochTime(this->year, this->month, this->day, this->hour, this->minute, this->second, TZ::unaware()).epochSeconds;
    }

    void DateStamp::str(ostream& out) const {
        this->year < 9999 ? out << string::fmt(fmt_04d, this->year) : out << "????";
        this->month > 0 || this->month < 13 ? out << string::fmt(fmt_02d, this->month) : out << "??";
        this->day > 0 || this->day < 32 ? out << string::fmt(fmt_02d, this->day) : out << "??";
        out << "T";
        this->hour < 24 ? out << string::fmt(fmt_02d, this->hour) : out << "??";
        this->minute < 60 ? out << string::fmt(fmt_02d, this->minute) : out << "??";;
        this->second < 60 ? out << string::fmt(fmt_02d, this->second) : out << "??";;
    }

    DateTime::Day DateStamp::dayOfWeek() const {
        auto days = days_from_civil(this->year, this->month, this->day);
        unsigned weekday = weekday_from_days(days);
        if (weekday == 0)
            weekday = 7;
        return DateTime::Day(weekday);
    }

    DateTime::Day DateStamp::getWeekDay(unsigned days) const {
        unsigned weekday = weekday_from_days(days);
        if (weekday == 0) weekday = 7;
        return (DateTime::Day)weekday;
    }

    unsigned DateStamp::weekNo() const {
        auto getWeekNo = [](unsigned weekdayFirst, unsigned yearDayIndex) {
            if (weekdayFirst <= 4) return (weekdayFirst + yearDayIndex - 1) / 7 + 1;
            else return (weekdayFirst + yearDayIndex - 1) / 7;
        };

        unsigned days = days_from_civil(this->year, this->month, this->day);
        unsigned daysFirst = days_from_civil(this->year, 1, 1);
        unsigned weekdayFirst = (unsigned)this->getWeekDay(daysFirst);
        unsigned yearDayIndex = days - daysFirst;

        unsigned weekNo = getWeekNo(weekdayFirst, yearDayIndex);
        if (weekNo != 0)
            return weekNo;

        if (is_leap(this->year - 1)) daysFirst -=  366;
        else daysFirst -= 365;

        weekdayFirst = (unsigned)this->getWeekDay(daysFirst);
        yearDayIndex = days - daysFirst;

        return getWeekNo(weekdayFirst, yearDayIndex);
    }

    unsigned DateStamp::dayOfYear() const {
        return days_from_civil(this->year, this->month, this->day) - days_from_civil(this->year, 1, 1) + 1;
    }

    unsigned DateStamp::daysInMonth() const {
        return last_day_of_month(this->year, this->month);
    }

    unsigned DateStamp::daysInYear() const {
        if (is_leap(this->year)) {
            return 366;
        }
        return 365;
    }

    void DateStamp::setWeekNo(unsigned n) {
        auto yearDayIndex = [](unsigned weekdayFirst, unsigned weekNo) {
            if (weekdayFirst <= 4) return ((int)weekNo - 1) * 7 - (int)weekdayFirst + 1;
            else return (int)weekNo * 7 - (int)weekdayFirst + 1;
        };

        unsigned daysFirst = days_from_civil(this->year, 1, 1);
        unsigned weekdayFirst = (unsigned)this->getWeekDay(daysFirst);

        this->day = 1;
        this->month = 1;
        int index = yearDayIndex(weekdayFirst, n);
        if (index > 0)
            this->incDay(index);
    }

    void DateStamp::incSecond(unsigned n) {
        this->second += n;
        while(this->second > 59) {
            this->incMinute(this->second/60);
            this->second %= 60;
        }
        this->second %= 60;
    }

    void DateStamp::incMinute(unsigned n) {
        this->minute += n;
        while(this->minute > 59) {
            this->incHour(this->minute/60);
            this->minute %= 60;
        }
        this->minute %= 60;
    }

     void DateStamp::incHour(unsigned n) {
        this->hour += n;
        while(this->hour > 23) {
            this->incDay(this->hour/24);
            this->hour %= 24;
        }
        this->hour %= 24;
    }

    void DateStamp::decDay(unsigned n) {
        this->day --;
        while (n > this->day) {
            n -= this->day;
            this->decMonth(1);
            this->day = last_day_of_month(this->year, this->month);
        }
        this->day -= n;
        this->day ++;
    }

    void DateStamp::incDay(unsigned n) {
        this->day --;
        {
            this->day += n;
            while(true) {
                if (this->day < 27) {
                    break;
                }
                unsigned last_day = last_day_of_month(this->year, this->month);
                if (this->day < last_day) {
                    break;
                }
                this->day -= last_day;
                this->incMonth(1);
            }
        }
        this->day ++;
    }

    void DateStamp::incWeek(unsigned n, DateTime::Day wkst) {
        auto dayOfWeek = this->dayOfWeek();
        this->incDay(DateTime::daysUntil(dayOfWeek, wkst));
        this->incDay(7 * (n - (dayOfWeek == wkst ? 0 : 1)));
    }

    void DateStamp::decMonth(unsigned n) {
        this->month --;
        while (n > this->month) {
            n -= month;
            this->month = 11;
            this->year --;
        }
        this->month -= n;
        this->month ++;
    }

    void DateStamp::incMonth(unsigned n) {
        this->month --;
        {
            this->month += n;
            if (this->month > 11) {
                this->incYear(this->month/12);
            }
            this->month %= 12;
        }
        this->month ++;
    }

    void DateStamp::incYear(unsigned n) {
        this->year += n;
    }
}
