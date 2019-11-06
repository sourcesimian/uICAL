#include "uICAL/cppstl.h"
#include "uICAL/tz.h"
#include "uICAL/epochtime.h"
#include "uICAL/datestamp.h"

namespace uICAL {
    static TZ::ptr NONE = TZ::ptr(new TZ());

    TZ::ptr TZ::none() {
        return NONE;
    }

    TZ::ptr TZ::init(int offsetMins) {
        return ptr(new TZ(offsetMins));
    }

    TZ::ptr TZ::init(const std::string tz) {
        return ptr(new TZ(tz));
    }

    TZ::TZ() {
        this->offsetMins = -1;
    }

    TZ::TZ(int offsetMins) {
        this->offsetMins = offsetMins;
    }

    TZ::TZ(const std::string tz) {
        this->offsetMins = TZ::parseOffset(tz);
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

    timestamp_t TZ::toUTC(timestamp_t timestamp) const {
        if (this->offsetMins == -1)
            return timestamp;
        return timestamp - (this->offsetMins * 60);
    }

    timestamp_t TZ::fromUTC(timestamp_t timestamp) const {
        if (this->offsetMins == -1)
            return timestamp;
        return timestamp + (this->offsetMins * 60);
    }

    void TZ::str(std::ostream& out) const {
        TZ::offsetAsString(out, this->offsetMins);
    }

    // TZ& TZ::operator= (const TZ &tz) {
    //     this->offset = tz.offset;
    //     return *this;
    // }


}