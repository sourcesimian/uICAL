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

    void DateSpan::str(std::ostream& out) const {
        auto dhms = to_dhms(this->spanSeconds);

        out << "P";
        if (std::get<0>(dhms)) out << std::get<0>(dhms) << "D";
        out << "T";
        if (std::get<1>(dhms)) out << std::get<1>(dhms) << "H";
        if (std::get<2>(dhms)) out << std::get<2>(dhms) << "M";
        if (std::get<3>(dhms)) out << std::get<3>(dhms) << "S";
    }

    std::ostream& operator << (std::ostream& out, const DateSpan& ds) {
        ds.str(out);
        return out;
    }
}
