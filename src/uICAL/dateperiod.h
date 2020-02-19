/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
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
            seconds_t spanSeconds;
    };

    std::ostream& operator << (std::ostream& out, const DatePeriod& ds);
}

#endif
