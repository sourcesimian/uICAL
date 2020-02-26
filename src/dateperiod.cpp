/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/datecalc.h"
#include "uICAL/dateperiod.h"

namespace uICAL {
    DatePeriod::DatePeriod() {
        this->spanSeconds = 0;
    }

    DatePeriod::DatePeriod(seconds_t seconds) {
        this->spanSeconds = seconds;
    }

    seconds_t DatePeriod::totalSeconds() const {
        return this->spanSeconds;
    }

    void DatePeriod::str(ostream& out) const {
        auto dhms = to_dhms(this->spanSeconds);

        out << "P";
        if (std::get<0>(dhms)) out << std::get<0>(dhms) << "D";
        out << "T";
        if (std::get<1>(dhms)) out << std::get<1>(dhms) << "H";
        if (std::get<2>(dhms)) out << std::get<2>(dhms) << "M";
        if (std::get<3>(dhms)) out << std::get<3>(dhms) << "S";
    }
}
