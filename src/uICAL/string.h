/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_h
#define uical_string_h

#ifdef ARDUINO

#include <WString.h>

namespace uICAL {
    class string : public String{

    };
}

#else

#include <string>

namespace uICAL {
    using string = std::string;
}

#endif
#endif
