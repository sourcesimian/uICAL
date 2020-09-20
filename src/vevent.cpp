/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/datetime.h"
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

        this->start = DateTime(dtStart->value + dtStart->getParam("TZID"), tzmap);
        this->end = DateTime(dtEnd->value + dtStart->getParam("TZID"), tzmap);

        this->summary = summary->value;

        this->rrule = new_ptr<RRule>(rRule->value, this->start);
    }

    void VEvent::str(ostream& out) const {
        out << "VEVENT: " << this->summary << uICAL::endl;
        out << " - start: " << this->start << uICAL::endl;
        out << " - end: " << this->end << uICAL::endl;
        out << " - rrule: " << this->rrule << uICAL::endl;
    }
}
