#ifndef uical_datespan_h
#define uical_datespan_h

#include "uICAL/datetime.h"

namespace uICAL {
    class DatePeriod {
        public:
            DatePeriod();
            DatePeriod(seconds_t span);

            seconds_t totalSeconds() const;

            void str(std::ostream& out) const;

        protected:
            // std::tuple<unsigned, unsigned, unsigned, unsigned> dhms(seconds_t spanSeconds) const;

            seconds_t spanSeconds;
    };

    std::ostream& operator << (std::ostream& out, const DatePeriod& ds);
}

#endif
