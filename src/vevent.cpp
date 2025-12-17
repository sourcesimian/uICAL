/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/error.h"
#include "uICAL/datetime.h"
#include "uICAL/dateperiod.h"
#include "uICAL/rrule.h"
#include "uICAL/vevent.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"

namespace uICAL {
    VEvent::VEvent(const VObject_ptr& obj, const TZMap_ptr& tzmap) {

        VLine_ptr dtStart = obj->getPropertyByName("DTSTART");
        VLine_ptr dtEnd = obj->getPropertyByName("DTEND");
        VLine_ptr rRule = obj->getPropertyByName("RRULE");
        VLine_ptr summary = obj->getPropertyByName("SUMMARY");

        // DTSTART is required per RFC 5545
        if (!dtStart) {
            throw ValueError("VEVENT missing required DTSTART property");
        }

        string tzid = dtStart->getParam("TZID");
        this->start = DateTime(dtStart->value + tzid, tzmap);

        // DTEND is optional - default to start + 1 day for all-day, 1 hour for timed
        if (dtEnd) {
            this->end = DateTime(dtEnd->value + tzid, tzmap);
        } else {
            seconds_t defaultDuration = (dtStart->value.length() == 8) ? 86400 : 3600;
            this->end = this->start + DatePeriod(defaultDuration);
        }

        // SUMMARY is optional per RFC 5545
        this->summary = summary ? summary->value : string();

        if (rRule == nullptr) {
            this->rrule = new_ptr<RRule>(string::none(), this->start);
        } else {
            this->rrule = new_ptr<RRule>(rRule->value, this->start);
        }
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }
}
