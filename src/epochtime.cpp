#include "uICAL/cppstl.h"
#include "uICAL/datecalc.h"
#include "uICAL/epochtime.h"

namespace uICAL {
    const seconds_t EpochTime::NaN = (unsigned)-1;

    EpochTime::EpochTime() {
        this->epochSeconds = NaN;
    }
    
    EpochTime::EpochTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, const TZ::ptr& tz) {
        auto epochDays = days_from_civil(year, month, day);
        this->epochSeconds = tz->toUTC(to_seconds(epochDays, hour, minute, second));
    }

    EpochTime::EpochTime(seconds_t seconds) {
        this->epochSeconds = seconds;
    }

    bool EpochTime::valid() const {
        return this->epochSeconds != NaN;
    }
    
    EpochTime::ymd_t EpochTime::ymd(const TZ::ptr tz) const {
        unsigned seconds = tz->fromUTC(this->epochSeconds);
        auto ymd = civil_from_days(seconds / (24 * 60 * 60));

        return ymd_t(std::get<0>(ymd), std::get<1>(ymd), std::get<2>(ymd));
    }

    EpochTime::ymdhms_t EpochTime::ymdhms(const TZ::ptr tz) const {
        seconds_t seconds = tz->fromUTC(this->epochSeconds);
        auto dhms = to_dhms(seconds);

        auto ymd = civil_from_days(std::get<0>(dhms));

        return ymdhms_t(
            std::get<0>(ymd), std::get<1>(ymd), std::get<2>(ymd),
            std::get<1>(dhms), std::get<2>(dhms), std::get<3>(dhms)
        );
    }

    seconds_t EpochTime::operator - (const EpochTime& other) const {
        return this->epochSeconds - other.epochSeconds;
    }

    bool EpochTime::operator > (const EpochTime& other) const {
        return this->epochSeconds > other.epochSeconds;
    }

    bool EpochTime::operator < (const EpochTime& other) const {
        return this->epochSeconds < other.epochSeconds;
    }

    bool EpochTime::operator <= (const EpochTime& other) const {
        return this->epochSeconds <= other.epochSeconds;
    }

    bool EpochTime::operator == (const EpochTime& other) const {
        return this->epochSeconds == other.epochSeconds;
    }

    bool EpochTime::operator != (const EpochTime& other) const {
        return this->epochSeconds != other.epochSeconds;
    }

    void EpochTime::str(std::ostream& out) const {
        out << this->epochSeconds;
    }

    std::ostream& operator << (std::ostream& out, const EpochTime& et) {
        et.str(out);
        return out;
    }
}
