/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/vline.h"
#include "uICAL/vlinestream.h"
#include "uICAL/util.h"
#include "uICAL/logging.h"

namespace uICAL {
    VLineStream::VLineStream(istream& ical)
    : ical(ical)
    {}

    const VLine::ptr VLineStream::next() {
        string line;
        if(line.readfrom(this->ical, '\n')) {
            line.rtrim();
            return VLine::init(line);
        }
        return VLine::init();
    }
}
