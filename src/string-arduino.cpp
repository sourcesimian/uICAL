/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifdef ARDUINO

#include "uICAL/string-arduino.h"

namespace uICAL {
    std::ostream& operator << (std::ostream& out, const string& st) {
        out << st.c_str();
        return out;
    }
}
#endif