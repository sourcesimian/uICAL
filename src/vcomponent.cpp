/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/vcomponent.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"

namespace uICAL {

    VComponent::ptr VComponent::init() {
        return VComponent::ptr(new VComponent());
    }

    VComponent::ptr VComponent::init(VLine::ptr beginline) {
        return VComponent::ptr(new VComponent(beginline));
    }

    VComponent::VComponent() {
    }

    VComponent::VComponent(VLine::ptr beginline) {
        this->name = beginline->value;
    }

    bool VComponent::empty() const {
        return this->name.empty();
    }

    const string& VComponent::getName() const {
        return this->name;
    }

    VLine::ptr VComponent::getPropertyByName(const string& name) {
        for (auto line : this->lines) {
            if (line->name == name) {
                return line;
            }
        }
        return VLine::init();
    }

    VComponent::vector VComponent::listComponents(const string& name) {
        VComponent::vector ret;
        for (auto child : this->children) {
            if (child->name == name) {
                ret.push_back(child);
            }
        }
        return ret;
    }

    void VComponent::str(ostream& out) const {
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
