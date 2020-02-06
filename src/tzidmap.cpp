#include "uICAL/cppstl.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/tzidmap.h"
#include "uICAL/tz.h"

namespace uICAL {
    TZIdMap::ptr TZIdMap::init(VComponent& calendar) {
        return TZIdMap::ptr(new TZIdMap(calendar));
    }

    TZIdMap::TZIdMap(VComponent& calendar) {
        auto timezones = calendar.listComponents("VTIMEZONE");
        for (auto it = timezones.begin(); it != timezones.end(); ++it) {
            std::string tzId = (*it)->getPropertyByName("TZID")->value;

            auto standards = (*it)->listComponents("STANDARD");
            for (auto it = standards.begin(); it != standards.end(); ++it) {

                std::string offset = (*it)->getPropertyByName("TZOFFSETFROM")->value;
                this->id_attrib_map[tzId].offset = TZ::parseOffset(offset);

                std::string name = (*it)->getPropertyByName("TZNAME")->value;
                this->id_attrib_map[tzId].name = name;
            }
        }
    }

    int TZIdMap::getOffset(const std::string tzId) {
        return this->id_attrib_map[tzId].offset;
    }

    std::string TZIdMap::getName(const std::string tzId) {
        return this->id_attrib_map[tzId].name;
    }

    void TZIdMap::str(std::ostream& out) const {
        for (auto i : this->id_attrib_map) {
            out << i.first << " : " << i.second.name << " : ";
            TZ::offsetAsString(out, i.second.offset);
            out << std::endl;
        }
    }

}
