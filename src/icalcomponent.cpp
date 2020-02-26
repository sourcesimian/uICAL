/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/icalcomponent.h"
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

    VComponentStream::VComponentStream(VLineStream& stm)
    : stm(stm)
    {}

    VComponent::ptr VComponentStream::next() {
        while(true) {
            VLine::ptr line = stm.next();
            if (line->empty()) {
                log_trace("End of next: %s", "Empty line");
                return VComponent::init();
            }
            if (line->name != "BEGIN") {
                log_error("BEGIN not found in: %s", line->as_str().c_str());
                throw ParseError(string("BEGIN not found in:") + line->as_str());
            }
            VComponent::ptr comp = VComponent::init(line);
            this->load(comp);
            return comp;
        }
    }

    void VComponentStream::load(VComponent::ptr comp) {
        while (true) {
            VLine::ptr line = stm.next();
            if (line->empty()) {
                log_error("%s", "Unexpected end of ICAL lines");
                throw ParseError("Unexpected end of ICAL lines");
            }
            if (line->name == "BEGIN") {
                VComponent::ptr subComp = VComponent::init(line);
                this->load(subComp);
                comp->children.push_back(subComp);
            }
            else if (line->name == "END") {
                if (comp->name != line->value) {
                    log_error("END mismatch \"%s\": %s", comp->name.c_str(), line->as_str().c_str());
                    throw ParseError(string("END mismatch for:") + line->as_str() + string(" ") + line->as_str());
                }
                log_trace("End of component: %s", line->as_str().c_str());
                return;
            }
            else {
                comp->lines.push_back(line);
            }
        }
    }
}
