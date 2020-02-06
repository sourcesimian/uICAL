#include "uICAL/cppstl.h"
#include "uICAL/tz.h"
#include "uICAL/epochtime.h"
#include "uICAL/datestamp.h"
#include "uICAL/error.h"

namespace uICAL {
    static TZ::ptr UNDEF = TZ::ptr(new TZ());
    static TZ::ptr UNAWARE = TZ::ptr(new TZ(false));

    TZ::ptr TZ::undef() {
        return UNDEF;
    }

    TZ::ptr TZ::unaware() {
        return UNAWARE;
    }

    TZ::ptr TZ::init(int offsetMins) {
        return ptr(new TZ(offsetMins));
    }

    TZ::ptr TZ::init(const std::string tz) {
        return ptr(new TZ(tz));
    }

    TZ::TZ() {
        this->offsetMins = -1;
        this->aware = false;
    }

    TZ::TZ(bool aware) {
        this->offsetMins = 0;
        this->aware = aware;
    }

    TZ::TZ(int offsetMins) {
        this->offsetMins = offsetMins;
        this->aware = true;
    }

    TZ::TZ(const std::string tz) {
        this->offsetMins = TZ::parseOffset(tz);
        this->aware = this->offsetMins != -1;
    }

    int TZ::parseOffset(const std::string tz) {
        if (tz == "Z") {
            return 0;
        }
        int offset = -1;
        if (tz.length() == 5) {
            char sign;
            unsigned tzH, tzM;

            // e.g.: +0200
            sscanf(tz.c_str(), "%c%02d%02d", &sign, &tzH, &tzM);

            offset = (tzH * 60) + tzM;
            if (sign == '-') {
                offset *= -1;
            }
        }
        return offset;
    }

    void TZ::offsetAsString(std::ostream& out, int offsetMins) {
        if (offsetMins != -1) {
            if (offsetMins == 0) {
                out << "Z";
            }
            else
            {
                if (offsetMins < 0) {
                    out << "-";
                    offsetMins *= -1;
                }
                else
                {
                    out << "+";
                }
                out << std::setfill('0') << std::setw(2) << offsetMins / 60;
                out << std::setfill('0') << std::setw(2) << offsetMins % 60;
            }
        }
    }

    seconds_t TZ::toUTC(seconds_t timestamp) const {
        if (this->offsetMins == -1)
            throw ImplementationError("Timezone not defined");

        return timestamp - (this->offsetMins * 60);
    }

    seconds_t TZ::fromUTC(seconds_t timestamp) const {
        if (this->offsetMins == -1)
            throw ImplementationError("Timezone not defined");

        return timestamp + (this->offsetMins * 60);
    }

    void TZ::str(std::ostream& out) const {
        if (this->offsetMins == -1)
            throw ImplementationError("Timezone not defined");
        if (!this->aware)
            return;
        TZ::offsetAsString(out, this->offsetMins);
    }

    std::ostream & operator << (std::ostream &out, const TZ::ptr& tz) {
        tz->str(out);
        return out;
    }
}