/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef ARDUINO
#include "uICAL/string-stl.h"

namespace uICAL {
    void string::_rtrim(string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }

    std::ostream& operator << (std::ostream& out, const string& st) {
        out << st.c_str();
        return out;
    }
}

#endif