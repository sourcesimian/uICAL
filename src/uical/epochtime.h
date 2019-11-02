#ifndef uical_epochtime_h
#define uical_epochtime_h

#include "uICAL/tz.h"

namespace uICAL {

    class EpochTime {
        public:
            EpochTime();
            EpochTime(unsigned year, unsigned month, unsigned day, unsigned hour, unsigned minute, unsigned second, const TZ::ptr& tz);
            
            using ymd_t = std::tuple<unsigned, unsigned, unsigned>;
            using dhms_t = std::tuple<unsigned, unsigned, unsigned, unsigned>;
            using ymdhms_t = std::tuple<unsigned, unsigned, unsigned, unsigned, unsigned, unsigned>;

            bool valid() const;
            
            ymd_t ymd(const TZ::ptr tz) const;
            dhms_t dhms(const TZ::ptr tz) const;
            ymdhms_t ymdhms(const TZ::ptr tz) const;

            bool operator > (const EpochTime& other) const;
            bool operator < (const EpochTime& other) const;
            bool operator <= (const EpochTime& other) const;
            bool operator == (const EpochTime& other) const;
            bool operator != (const EpochTime& other) const;
            // operator bool() const { return this->epochSeconds != (unsigned)-1; }

            void str(std::ostream& out) const;

            using epoch_t = unsigned long long int;
            epoch_t epochSeconds;

        protected:
            epoch_t toSeconds(unsigned day, unsigned hour, unsigned minute, unsigned second);
            static const epoch_t NaN;
    };

    std::ostream & operator << (std::ostream &out, const EpochTime& et);
}
#endif
