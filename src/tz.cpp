/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/tz.h"
#include "uICAL/datestamp.h"
#include "uICAL/epochtime.h"
#include "uICAL/tzmap.h"

namespace uICAL {
    static TZ_ptr UNDEF = new_ptr<TZ>();
    static TZ_ptr UNAWARE = new_ptr<TZ>(false);

    TZ_ptr TZ::undef() {
        return UNDEF;
    }

    TZ_ptr TZ::unaware() {
        return UNAWARE;
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

    TZ::TZ(const string& tz) {
        this->offsetMins = TZ::parseOffset(tz);
        this->aware = true;
    }

    TZ::TZ(const string& tz, const TZMap_ptr& tzmap)
    : idmap(tzmap)
    {
        string id = this->idmap->findId(tz);
        if (!id.empty()) {
            this->id = id;
            this->aware = true;
        }
        else {
            this->offsetMins = TZ::parseOffset(tz);
            this->aware = true;
        }
    }

    bool TZ::is_aware() const {
        return this->aware;
    }

    int TZ::parseOffset(const string& tz) {
        if (tz == "Z") {
            return 0;
        }
        try {
            if (tz.length() == 5) {
                char sign;
                unsigned tzH, tzM;

                // e.g.: +0200
                sign = tz.at(0);
                tzH = tz.substr(1, 2).as_int();
                tzM = tz.substr(3, 2).as_int();

                int offset = (tzH * 60) + tzM;
                if (sign == '-') {
                    offset *= -1;
                }
                return offset;
            }
        }
        catch (std::invalid_argument const &e)
        {}
        catch (std::out_of_range const &e)
        {}
        throw ValueError("Bad timezone: \"" + tz + "\"");
    }

    void TZ::offsetAsString(ostream& out, int offsetMins) {
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
                out << string::fmt(fmt_02d, offsetMins / 60);
                out << string::fmt(fmt_02d, offsetMins % 60);
            }
        }
    }

    int TZ::offset() const {
        if (!this->id.empty()) {
            return this->idmap->getOffset(this->id);
        }
        if (this->offsetMins == -1)
            throw ImplementationError("Timezone not defined");
        return this->offsetMins;
    }

    seconds_t TZ::toUTC(seconds_t timestamp) const {
        return timestamp - (this->offset() * 60);
    }

    seconds_t TZ::fromUTC(seconds_t timestamp) const {
        return timestamp + (this->offset() * 60);
    }

    void TZ::str(ostream& out) const {
        if (!this->id.empty()) {
            out << this->idmap->getName(this->id);
            return;
        }

        if (this->offsetMins == -1)
            throw ImplementationError("Timezone not defined");
        if (!this->aware)
            return;
        TZ::offsetAsString(out, this->offsetMins);
    }
}
