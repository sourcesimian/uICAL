#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/tzmap.h"
#include "uICAL/tz.h"

namespace uICAL {
    TZMap::ptr TZMap::init() {
        return TZMap::ptr(new TZMap());
    }

    TZMap::ptr TZMap::init(VComponent& calendar) {
        return TZMap::ptr(new TZMap(calendar));
    }

    TZMap::TZMap() {
    }

    TZMap::TZMap(VComponent& calendar) {
        auto timezones = calendar.listComponents("VTIMEZONE");
        for (auto it = timezones.begin(); it != timezones.end(); ++it) {
            std::string tzId = (*it)->getPropertyByName("TZID")->value;

            auto standards = (*it)->listComponents("STANDARD");
            for (auto it = standards.begin(); it != standards.end(); ++it) {

                std::string offset = (*it)->getPropertyByName("TZOFFSETFROM")->value;
                // this->id_attrib_map[tzId].offset = TZ::parseOffset(offset);

                std::string name = (*it)->getPropertyByName("TZNAME")->value;
                // this->id_attrib_map[tzId].name = name;

                this->add(tzId, name, offset);
            }
        }
    }

    void TZMap::add(const std::string id, const std::string name, const std::string tz) {
        this->id_attrib_map[id].offset = TZ::parseOffset(tz);
        this->id_attrib_map[id].name = name;
    }

    std::string TZMap::findId(const std::string nameOrId) const {
        for (auto i : this->id_attrib_map) {
            if (i.second.name == nameOrId || i.first == nameOrId) {
                return i.first;
            }
        }

        return std::string();
    }

    int TZMap::getOffset(const std::string tzId) {
        return this->id_attrib_map[tzId].offset;
    }

    std::string TZMap::getName(const std::string tzId) {
        return this->id_attrib_map[tzId].name;
    }

    void TZMap::str(std::ostream& out) const {
        for (auto i : this->id_attrib_map) {
            out << i.first << " : " << i.second.name << " : ";
            TZ::offsetAsString(out, i.second.offset);
            out << std::endl;
        }
    }

}