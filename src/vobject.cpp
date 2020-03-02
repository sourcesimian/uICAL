/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/vline.h"
#include "uICAL/vobject.h"

namespace uICAL {
    VObject::VObject()
    {}

    bool VObject::empty() const {
        return this->name.empty();
    }

    const string& VObject::getName() const {
        return this->name;
    }


    VLine_ptr VObject::getPropertyByName(const string& name) const {
        for (auto line : this->lines) {
            if (line->name == name) {
                return line;
            }
        }
        return new_ptr<VLine>();
    }

    VObject::vector VObject::listObjects(const string& name) const {
        VObject::vector ret;
        for (auto child : this->children) {
            if (child->name == name) {
                ret.push_back(child);
            }
        }
        return ret;
    }

    void VObject::str(ostream& out) const {
        out << "BEGIN:" << this->name << uICAL::endl;
        for (auto line : this->lines) {
            line->str(out);
            out << uICAL::endl;
        }

        for (auto child : this->children) {
            child->str(out);
        }

        out << "END:" << this->name << uICAL::endl;
    }
}
