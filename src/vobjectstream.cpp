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

    VObject_ptr VObjectStream::nextChild() {
        VObject_ptr child;
        if (!this->child) {
            child = this->nextObj(this->obj);
        }
        else {
            child = this->nextObj(this->child);
        }
        this->child = child;
        return child;
    }

    VObject_ptr VObjectStream::nextObj(VObject_ptr& obj) {
        while(true) {
            VLine_ptr line = stm.next();
            if (line->empty()) {
                log_error("%s", "Unexpected end of ICAL lines");
                throw ParseError("Unexpected end of ICAL");
            }
            if (line->name == "BEGIN") {
                VObject_ptr child = new_ptr<VObject>();
                child->name = line->value;
                log_debug("Child obj: %s", child->name.c_str());
                return child;
            }
            else if (line->name == "END") {
                if (obj->name == line->value) {
                    log_trace("End of component: %s", line->as_str().c_str());
                    return new_ptr<VObject>();
                }
                if (this->obj->name == line->value) {
                    log_trace("End of stream: %s", line->as_str().c_str());
                    return new_ptr<VObject>();
                }
                log_error("END mismatch \"%s\": %s", obj->name.c_str(), line->as_str().c_str());
                throw ParseError(string("END mismatch for:") + obj->name.c_str() + string(" ") + line->as_str());
            }
            else {
                obj->lines.push_back(line);
            }
        }
    }

    void VObjectStream::loadChild() {
        this->loadObj(this->obj, this->child);
    }

    void VObjectStream::loadObj(VObject_ptr& obj, VObject_ptr& child) {
        for (;;) {
            VObject_ptr nextObj = this->nextObj(child);
            if (!nextObj->empty()) {
                this->loadObj(child, nextObj);
            }
            else {
                break;
            }
        }
        obj->children.push_back(child);
    }

    // void VObjectStream::skipChild(VObject_ptr& child) {
    //     while (true) {
    //         VLine_ptr line = stm.next();
    //         if (line->empty()) {
    //             log_error("%s", "Unexpected end of ICAL lines");
    //             throw ParseError("Unexpected end of ICAL");
    //         }
    //         if (line->name == "END" && child->name == line->value) {
    //             return;
    //         }
    //     }
    // }
}
