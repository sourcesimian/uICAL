/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/datecalc.h"
#include "uICAL/datetime.h"
#include "uICAL/epochtime.h"
#include "uICAL/datestamp.h"
#include "uICAL/dateperiod.h"
#include "uICAL/tz.h"
#include "uICAL/tzmap.h"

namespace uICAL {
    DateTime::DateTime() {
        this->tz = TZ::undef();
    }

    DateTime::DateTime(const DateStamp& ds, const TZ_ptr& tz) {
        this->construct(ds, tz);
    }

    DateTime::DateTime(const string& datetime) {
        this->construct(datetime, new_ptr<TZMap>());
    }

    DateTime::DateTime(const string& datetime, const TZMap_ptr& tzmap) {
        this->construct(datetime, tzmap);
    }

    DateTime::DateTime(seconds_t epochSeconds) {
        this->epochtime = EpochTime(epochSeconds);
        this->tz = new_ptr<TZ>("Z");
    }

    DateTime::DateTime(seconds_t epochSeconds, const TZ_ptr& tz) {
        this->epochtime = EpochTime(epochSeconds);
        this->tz = tz;
    }

    void DateTime::construct(const string& datetime, const TZMap_ptr& tzmap) {
        if (datetime.length() < 15)
            throw ValueError(string("Bad datetime: \"") + datetime + "\"");

        DateStamp ds(datetime.substr(0, 15));

        if (datetime.length() > 15) {
            this->tz = new_ptr<TZ>(datetime.substr(15), tzmap);
        }
        else {
            this->tz = TZ::unaware();
        }

        this->construct(ds, tz);
    }

    void DateTime::construct(const DateStamp& ds, const TZ_ptr& tz) {
        this->epochtime = EpochTime(
            ds.year, ds.month, ds.day, ds.hour, ds.minute, ds.second,
            tz
        );
        this->tz = tz;
    }

    bool DateTime::valid() const {
        return this->epochtime.valid();
    }

    DateStamp DateTime::datestamp() const {
        auto ymdhms = this->epochtime.ymdhms(this->tz);

        return DateStamp(
            std::get<0>(ymdhms), std::get<1>(ymdhms), std::get<2>(ymdhms),
            std::get<3>(ymdhms), std::get<4>(ymdhms), std::get<5>(ymdhms)
        );
    }

    DateStamp DateTime::datestamp(const TZ_ptr& tz) const {
        auto ymdhms = this->epochtime.ymdhms(tz);

        return DateStamp(
            std::get<0>(ymdhms), std::get<1>(ymdhms), std::get<2>(ymdhms),
            std::get<3>(ymdhms), std::get<4>(ymdhms), std::get<5>(ymdhms)
        );
    }

    DateTime& DateTime::operator = (const DateTime& other) {
        this->tz = other.tz;
        this->epochtime = other.epochtime;
        return *this;
    }

    void DateTime::assert_awareness(const DateTime& other) const {
        if (this->tz->is_aware() != other.tz->is_aware()) {
            throw TZAwarenessConflictError(this->as_str() + " <> " + other.as_str());
        }
    }

    DatePeriod DateTime::operator - (const DateTime& other) const {
        this->assert_awareness(other);
        return DatePeriod(this->epochtime.epochSeconds - other.epochtime.epochSeconds);
    }

    DatePeriod DateTime::operator + (const DateTime& other) const {
        this->assert_awareness(other);
        return DatePeriod(this->epochtime.epochSeconds + other.epochtime.epochSeconds);
    }

    DateTime DateTime::operator + (const DatePeriod& dp) const {
        return DateTime(this->epochtime.epochSeconds + dp.totalSeconds(), this->tz);
    }

    DateTime DateTime::operator - (const DatePeriod& dp) const {
        return DateTime(this->epochtime.epochSeconds - dp.totalSeconds(), this->tz);
    }

    bool DateTime::operator > (const DateTime& other) const {
        this->assert_awareness(other);
        return this->epochtime > other.epochtime;
    }

    bool DateTime::operator < (const DateTime& other) const {
        this->assert_awareness(other);
        return this->epochtime < other.epochtime;
    }

    bool DateTime::operator <= (const DateTime& other) const {
        this->assert_awareness(other);
        return this->epochtime <= other.epochtime;
    }

    bool DateTime::operator == (const DateTime& other) const {
        this->assert_awareness(other);
        return this->epochtime == other.epochtime;
    }

    void DateTime::str(ostream& out) const {
        auto ymdhms = this->epochtime.ymdhms(this->tz);

        out << string::fmt(fmt_04d, std::get<0>(ymdhms));
        out << string::fmt(fmt_02d, std::get<1>(ymdhms));
        out << string::fmt(fmt_02d, std::get<2>(ymdhms));
        out << "T";
        out << string::fmt(fmt_02d, std::get<3>(ymdhms));
        out << string::fmt(fmt_02d, std::get<4>(ymdhms));
        out << string::fmt(fmt_02d, std::get<5>(ymdhms));

        this->tz->str(out);
    }

    unsigned DateTime::daysUntil(DateTime::Day today, DateTime::Day then) {
        return ((int)today <= (int)then ? 0 : 7) + (int)then - (int)today;
    }

    unsigned DateTime::daysUntil(DateTime::Day today, int index, DateTime::Day then, unsigned span) {
        if (index > 0) {
            span = 0;
        }
        else {
            index ++;
            today = (DateTime::Day)((((int)today - 1 + span) % 7) + 1);
        }

        return span +
                ((index - ((unsigned)then >= (unsigned)today ? 1 : 0)) * 7) +
                ((unsigned)then - (unsigned)today);
    }

    DateTime::Day DateTime::dayOfWeekAfter(DateTime::Day today, unsigned days) {
        return DateTime::Day( ((int)today + days - 1) % 7 + 1 );
    }

    ostream& operator << (ostream& out, const DateTime::Day& day) {
        if (day == DateTime::Day::MON) out << "MO";
        else if (day == DateTime::Day::TUE) out << "TU";
        else if (day == DateTime::Day::WED) out << "WE";
        else if (day == DateTime::Day::THU) out << "TH";
        else if (day == DateTime::Day::FRI) out << "FR";
        else if (day == DateTime::Day::SAT) out << "SA";
        else if (day == DateTime::Day::SUN) out << "SU";
        else out << "??";
        return out;
    }
}
