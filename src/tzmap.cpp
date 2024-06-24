/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/tzmap.h"
#include "uICAL/tz.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"

namespace uICAL {
    TZMap::TZMap() {
    }

    void TZMap::add(const VObject_ptr& timezone) {
        string tzId = timezone->getPropertyByName("TZID")->value;

        auto standards = timezone->listObjects("STANDARD");
        for (auto standard : standards) {

            string offset = standard->getPropertyByName("TZOFFSETFROM")->value;
            string name = tzId;

            this->add(tzId, name, offset);
        }
    }

    void TZMap::add(const string& id, const string& name, const string& tz) {
        this->id_attrib_map[id].offset = TZ::parseOffset(tz);
        this->id_attrib_map[id].name = name;
    }

    string TZMap::findId(const string& nameOrId) const {
        for (auto i : this->id_attrib_map) {
            if (i.second.name == nameOrId || i.first == nameOrId) {
                return i.first;
            }
        }

        return string();
    }

    int TZMap::getOffset(const string& tzId) {
        return this->id_attrib_map[tzId].offset;
    }

    string TZMap::getName(const string& tzId) {
        return this->id_attrib_map[tzId].name;
    }

    void TZMap::str(ostream& out) const {
        for (auto i : this->id_attrib_map) {
            out << i.first << " : " << i.second.name << " : ";
            TZ::offsetAsString(out, i.second.offset);
            out << uICAL::endl;
        }
    }
}
