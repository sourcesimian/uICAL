/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifdef ARDUINO

#include "uICAL/string-arduino.h"


namespace uICAL {
    const char* endl = "\n";

    std::ostream& operator << (std::ostream& out, const string& st) {
        out << st.c_str();
        return out;
    }

    int to_int(const string& st) {
        return st.toInt();
    }

    bool getline(std::istream& is, string& st, char token)
    {
        std::string tmp;
        if(std::getline(is, tmp, token)) {
            st = tmp.c_str();
            return true;
        }
        return false;
    }

    void rtrim(string& st) {
        st.trim();
    }

    // string fmt( const char * format, ...) {
    //     string ret;
    //     ret.reserve(10);
    //     va_list args;
    //     va_start(args, b);
    //     vsprintf (ret.c_str(), format, args);
    //     va_end(args);
    //     return string();
    // }

}

#endif
