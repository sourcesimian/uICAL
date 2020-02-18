/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
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

    TZ::ptr TZ::init(const std::string tz, const TZMap::ptr& tzmap) {
        return ptr(new TZ(tz, tzmap));
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
        this->aware = true;
    }

    TZ::TZ(const std::string tz, const TZMap::ptr& tzmap)
    : idmap(tzmap)
    {
        std::string id = this->idmap->findId(tz);
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

    int TZ::parseOffset(const std::string tz) {
        if (tz == "Z") {
            return 0;
        }
        try {
            if (tz.length() == 5) {
                char sign;
                unsigned tzH, tzM;

                // e.g.: +0200
                sign = tz.at(0);
                tzH = std::atoi(tz.substr(1, 2).c_str());
                tzM = std::atoi(tz.substr(3, 2).c_str());

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

    void TZ::str(std::ostream& out) const {
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

    std::string TZ::str() const {
        std::ostringstream out;
        this->str(out);
        return out.str();
    }

    std::ostream& operator << (std::ostream& out, const TZ::ptr& tz) {
        tz->str(out);
        return out;
    }
}