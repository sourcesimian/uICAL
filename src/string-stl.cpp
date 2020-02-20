/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef ARDUINO

#include "uICAL/string-stl.h"

namespace uICAL {

    std::ostream& operator << (std::ostream& out, const string& st) {
        out << st.c_str();
        return out;
    }

    int to_int(const string& st)
    {
        return std::atoi(st.c_str());
    }

    bool getline(std::istream& is, string& st, char token) {
        if(std::getline(is, st, token)) {
            return true;
        }
        return false;
    }

    void rtrim(string& st) {
        st.erase(std::find_if(st.rbegin(), st.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), st.end());
    }
}

#endif
