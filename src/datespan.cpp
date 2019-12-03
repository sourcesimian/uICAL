#include "uICAL/cppstl.h"
#include "uICAL/datecalc.h"
#include "uICAL/datespan.h"

namespace uICAL {
    DateSpan::DateSpan() {
        this->spanSeconds = 0;
    }

    DateSpan::DateSpan(seconds_t seconds) {
        this->spanSeconds = seconds;
    }

    seconds_t DateSpan::totalSeconds() const {
        return this->spanSeconds;
    }
}
