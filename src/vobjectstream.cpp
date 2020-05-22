/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/vevent.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"
#include "uICAL/vobject.h"
#include "uICAL/vobjectstream.h"

namespace uICAL {
    VObjectStream::VObjectStream(VLineStream& stm, lineP_t useLine)
    : stm(stm)
    , useLine(useLine)
    {}

    string VObjectStream::nextObjectName() {
        VLine_ptr line = this->stm.next();

        if (!line) {
            log_error("Parse error: %s", "Empty stream");
            throw ParseError(string("Parse error, empty stream"));
        }
        if (line->name == "END") {
            return string::none();
        }
        if (line->name != "BEGIN") {
            log_error("Parse error, expected BEGIN: %s", line->as_str().c_str());
            throw ParseError(string("Parse error, expected BEGIN: ") + line->as_str());
        }

        return line->value;
    }

    void VObjectStream::loadObject(string objName, VObject_ptr& obj, bool recurse) {
        while(true) {
            VLine_ptr line = this->stm.next();
            if (!line) {
                log_error("%s", "Parse error, unexpected end of ICAL");
                throw ParseError("Parse error, unexpected end of ICAL");
            }
            if (line->name == "BEGIN") {
                stm.repeatLine();
                if (recurse || obj == nullptr) {
                    string childName = this->nextObjectName();
                    VObject_ptr child = nullptr;
                    if (this->useLine(childName, string::none())) {
                        child = new_ptr<VObject>();
                        child->name = childName;
                    }
                    loadObject(childName, child, true);
                    if (obj && child) {
                        obj->children.push_back(child);
                    }
                }
                else {
                    return;
                }
            }
            else if (line->name == "END") {
                if (objName == line->value) {
                    log_trace("End of component: %s", line->as_str().c_str());
                    return;
                }
                log_error("END mismatch \"%s\": %s", objName.c_str(), line->as_str().c_str());
                throw ParseError(string("END mismatch for:") + objName.c_str() + string(" ") + line->as_str());
            }
            else {
                if (obj && this->useLine(obj->getName(), line->name)) {
                    obj->lines.push_back(line);
                }
            }
        }
    }

    VObject_ptr VObjectStream::nextObject(bool recurse) {
        while(true) {
            string objName = this->nextObjectName();
            if (objName.empty()) {
                return nullptr;
            }
            VObject_ptr obj = nullptr;
            if (this->useLine(objName, string())) {
                obj = new_ptr<VObject>();
                obj->name = objName;
            }
            loadObject(objName, obj, recurse);
            if (obj) {
                return obj;
            }
        }
    }
}
