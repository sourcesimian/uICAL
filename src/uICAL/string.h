/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_h
#define uical_string_h

#ifdef ARDUINO
#include "uICAL/string-arduino.h"
#include <Arduino.h>
#else
#include "uICAL/string-stl.h"
#include <stdlib.h>
#endif



namespace uICAL {
    extern const char * fmt_04d;
    extern const char * fmt_02d;

    template<typename... Args>
    string fmt(const char * f, Args... args) {
        char ret[10];
        sprintf(ret, f, args...);
        return string(ret);
    }

}
#endif
