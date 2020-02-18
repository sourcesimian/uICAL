/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
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
        for (auto timezone : timezones) {
            std::string tzId = timezone->getPropertyByName("TZID")->value;

            auto standards = timezone->listComponents("STANDARD");
            for (auto standard : standards) {

                std::string offset = standard->getPropertyByName("TZOFFSETFROM")->value;
                std::string name = standard->getPropertyByName("TZNAME")->value;

                this->add(tzId, name, offset);
            }
        }
    }

    void TZMap::add(const std::string& id, const std::string& name, const std::string& tz) {
        this->id_attrib_map[id].offset = TZ::parseOffset(tz);
        this->id_attrib_map[id].name = name;
    }

    std::string TZMap::findId(const std::string& nameOrId) const {
        for (auto i : this->id_attrib_map) {
            if (i.second.name == nameOrId || i.first == nameOrId) {
                return i.first;
            }
        }

        return std::string();
    }

    int TZMap::getOffset(const std::string& tzId) {
        return this->id_attrib_map[tzId].offset;
    }

    std::string TZMap::getName(const std::string& tzId) {
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
