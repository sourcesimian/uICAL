#include "uICAL/cppstl.h"
#include "uICAL/datecalc.h"
#include "uICAL/epochtime.h"

namespace uICAL {
    const EpochTime::epoch_t EpochTime::invalid = (unsigned)-1;

    EpochTime::EpochTime() {
        this->epochSeconds = invalid;
    }
    
    EpochTime::EpochTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, const TZ::ptr& tz) {
        std::ignore = tz;
        auto epochDays = days_from_civil(year, month, day);
        this->epochSeconds = this->toSeconds(epochDays, hour, minute, second);
    }

    bool EpochTime::empty() const {
        return this->epochSeconds == invalid;
    }
    
    EpochTime::dhms_t EpochTime::dhms(const TZ::ptr tz) const {
        std::ignore = tz;
        epoch_t seconds = this->epochSeconds;
        unsigned hour, minute, second;

        second = seconds % 60;
        seconds /= 60;

        minute = seconds % 60;
        seconds /= 60;

        hour = seconds % 24;
        seconds /= 24;

        return dhms_t(seconds, hour, minute, second);
    }

    EpochTime::ymd_t EpochTime::ymd(const TZ::ptr tz) const {
        std::ignore = tz;
        unsigned seconds = this->epochSeconds;
        auto ymd = civil_from_days(seconds / (24 * 60 * 60));

        return ymd_t(std::get<0>(ymd), std::get<1>(ymd), std::get<2>(ymd));
    }

    EpochTime::ymdhms_t EpochTime::ymdhms(const TZ::ptr tz) const {
        auto dhms = this->dhms(tz);
        auto ymd = civil_from_days(std::get<0>(dhms));

        return ymdhms_t(
            std::get<0>(ymd), std::get<1>(ymd), std::get<2>(ymd),
            std::get<1>(dhms), std::get<2>(dhms), std::get<3>(dhms)
        );
    }

    EpochTime::epoch_t EpochTime::toSeconds(unsigned day, unsigned hour, unsigned minute, unsigned second) {
        return (epoch_t)day * 60 * 60 * 24 + hour * 60 * 60 + minute * 60 + second;
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

    std::ostream & operator << (std::ostream &out, const EpochTime& et) {
        et.str(out);
        return out;
    }
}
