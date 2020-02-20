/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_types_h
#define uical_types_h

#ifdef ARDUINO
#include "uICAL/string-arduino.h"
#else
#include "uICAL/string-stl.h"
#endif

namespace uICAL {
    using seconds_t = long long int;
}    

#endif
