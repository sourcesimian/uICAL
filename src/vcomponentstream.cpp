/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/vcomponent.h"
#include "uICAL/vcomponentstream.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"

namespace uICAL {

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
                throw ParseError("Unexpected end of ICAL");
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
