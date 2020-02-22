/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_datespan_h
#define uical_datespan_h

#include "uICAL/base.h"
#include "uICAL/datetime.h"

namespace uICAL {
    class DatePeriod : public Base {
        public:
            DatePeriod();
            DatePeriod(seconds_t span);

            seconds_t totalSeconds() const;

            void str(ostream& out) const;

        protected:
            seconds_t spanSeconds;
    };

    ostream& operator << (ostream& out, const DatePeriod& ds);
}

#endif
