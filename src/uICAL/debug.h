/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifdef DEBUG_OUT

#ifdef ARDUINO

#include <Arduino.h>

#define debug(M) \
    do { \
        Serial.println(String("[debug] ") + string(M).c_str()); \
    } while(0)
#else

#include <iostream>

#define debug(M) \
    do { \
        std::cout << "[debug] " << string(M) << uICAL::endl; \
    } while(0)
#endif

#else

#define debug(M) do {} while(0)

#endif