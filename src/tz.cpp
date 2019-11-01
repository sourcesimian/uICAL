#include "uICAL/cppstl.h"
#include "uICAL/tz.h"
#include "uICAL/datestamp.h"

namespace uICAL {
    static TZ::ptr NONE = TZ::ptr(new TZ());

    TZ::ptr TZ::none() {
        return NONE;
    }

    TZ::ptr TZ::init(const std::string tz) {
        return ptr(new TZ(tz));
    }

    TZ::TZ() {
        this->offset = -1;
    }

    TZ::TZ(const std::string tz) {
        if (tz == "Z") {
            this->offset = 0;
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
        this->offset = offset;
    }

    void TZ::str(std::ostream& out) const {
        if (this->offset != -1) {
            if (this->offset == 0) {
                out << "Z";
            }
            else
            {
                int offset = this->offset;
                if (offset < 0) {
                    out << "-";
                    offset *= -1;
                }
                else
                {
                    out << "+";
                }
                out << std::setfill('0') << std::setw(2) << offset / 60;
                out << std::setfill('0') << std::setw(2) << offset % 60;
            }
        }
    }

    TZ& TZ::operator= (const TZ &tz) {
        this->offset = tz.offset;
        return *this;
    }

}