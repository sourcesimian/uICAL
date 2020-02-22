/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/base.h"

namespace uICAL {
    
    string Base::as_str() const {
        ostream stm;
        this->str(stm);
        return stm;
    }
}
