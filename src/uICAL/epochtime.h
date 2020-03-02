/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_epochtime_h
#define uical_epochtime_h

#include "uICAL/base.h"

namespace uICAL {
    class TZ;

    class EpochTime : public Base {
        public:
            EpochTime();
            EpochTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, const TZ_ptr& tz);
            EpochTime(seconds_t seconds);

            using ymd_t = std::tuple<unsigned, unsigned, unsigned>;
            using ymdhms_t = std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned, unsigned>;

            bool valid() const;

            ymd_t ymd(const TZ_ptr tz) const;
            ymdhms_t ymdhms(const TZ_ptr tz) const;

            seconds_t operator - (const EpochTime& other) const;

            bool operator > (const EpochTime& other) const;
            bool operator < (const EpochTime& other) const;
            bool operator <= (const EpochTime& other) const;
            bool operator == (const EpochTime& other) const;
            bool operator != (const EpochTime& other) const;
            // operator bool() const { return this->epochSeconds != (unsigned)-1; }

            // EpochTime offset(int seconds) const;
            void str(ostream& out) const;

            seconds_t epochSeconds;

        protected:
            static const seconds_t NaN;
    };
}
#endif
