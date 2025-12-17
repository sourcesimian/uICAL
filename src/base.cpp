/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/base.h"

namespace uICAL {

    // Global invalid date handling mode (default: SKIP for RFC 5545 compliance)
    static InvalidDateMode g_invalidDateMode = InvalidDateMode::SKIP;

    InvalidDateMode getInvalidDateMode() {
        return g_invalidDateMode;
    }

    void setInvalidDateMode(InvalidDateMode mode) {
        g_invalidDateMode = mode;
    }

    string Base::as_str() const {
        ostream stm;
        this->str(stm);
        return stm;
    }

    ostream& operator << (ostream& out, const Base& b) {
        b.str(out);
        return out;
    }
}
