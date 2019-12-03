#ifndef uical_datespan_h
#define uical_datespan_h

#include "uICAL/datetime.h"

namespace uICAL {
    class DateSpan {
        public:
            DateSpan();
            DateSpan(seconds_t span);

            seconds_t totalSeconds() const;

        protected:
            seconds_t spanSeconds;
    };
}

#endif
