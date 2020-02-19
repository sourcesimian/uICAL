/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/string.h"

#ifdef ARDUINO

namespace uICAL {
    using string = String;
}

#else

namespace uICAL {
    // Nothing using string
}

#endif