/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"

namespace uICAL {
    VLineStream::VLineStream(istream& ical)
    : ical(ical)
    , currentLine(nullptr)
    , repeat(false)
    {}

    void VLineStream::repeatLine() {
        this->repeat = true;
    }

    const VLine_ptr VLineStream::next() {
        if (!this->repeat || this->currentLine == nullptr) {
            string line;
            if(line.readfrom(this->ical, '\n')) {
                line.rtrim();
                this->currentLine = new_ptr<VLine>(line);
            }
            else {
                this->currentLine = nullptr;
            }
        }
        this->repeat = false;
        return this->currentLine;
    }
}
