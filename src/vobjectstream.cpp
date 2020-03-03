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
    VObjectStream::VObjectStream(VLineStream& stm, VObject_ptr& obj)
    : stm(stm)
    , obj(obj)
    {
        VLine_ptr line = stm.next();
        if (line->empty()) {
            log_error("Parse error: %s", "Empty stream");
            throw ParseError(string("Parse error: empty stream"));
        }
        if (line->name != "BEGIN") {
            log_error("Parse error: Bad first line: %s", line->as_str().c_str());
            throw ParseError(string("Parse error: Bad first line: ") + line->as_str());
        }
        this->obj->name = line->value;
    }

    const string& VObjectStream::nextChild() {
        VLine_ptr line;
        if (!this->child) {
            line = this->nextObj(this->obj, false);
        }
        else {
            line = this->nextObj(this->child, false);
        }
        if (!line) {
            return string::none();
        }
        this->child = new_ptr<VObject>();
        this->child->name = line->value;
        return this->child->name;
    }

    VObject_ptr VObjectStream::loadChild() {
        this->loadObj(this->obj, this->child, false);
        return this->child;
    }

    void VObjectStream::skipChild() {
        this->loadObj(this->obj, this->child, true);
    }

    VLine_ptr VObjectStream::nextObj(VObject_ptr& obj, bool skip) {
        while(true) {
            VLine_ptr line = this->stm.next();
            if (line->empty()) {
                log_error("%s", "Unexpected end of ICAL lines");
                throw ParseError("Unexpected end of ICAL");
            }
            if (line->name == "BEGIN") {
                return line;
            }
            else if (line->name == "END") {
                if (obj->name == line->value) {
                    log_trace("End of component: %s", line->as_str().c_str());
                    return nullptr;
                }
                if (this->obj->name == line->value) {
                    log_trace("End of stream: %s", line->as_str().c_str());
                    return nullptr;
                }
                log_error("END mismatch \"%s\": %s", obj->name.c_str(), line->as_str().c_str());
                throw ParseError(string("END mismatch for:") + obj->name.c_str() + string(" ") + line->as_str());
            }
            else {
                if (!skip) {
                    obj->lines.push_back(line);
                }
            }
        }
    }

    void VObjectStream::loadObj(VObject_ptr& obj, VObject_ptr& child, bool skip) {
        for (;;) {
            VLine_ptr line = this->nextObj(child, skip);
            if (!line) {
                break;
            }
            VObject_ptr nextObj = new_ptr<VObject>();
            nextObj->name = line->value;
            this->loadObj(child, nextObj, skip);
        }
        if (!skip) {
            obj->children.push_back(child);
        }
    }
}
