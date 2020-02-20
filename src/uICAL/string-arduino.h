/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_arduino_h
#define uical_string_arduino_h

#include <WString.h>
#include <sstream>

namespace uICAL {
    class string : public String{
        public:
            using pos = unsigned int;
            
            string() : String() {}
            string(const String& b) : String(b) {}
            string(const char* st) : String(st) {}
            string(const StringSumHelper& ssh) : String(ssh) {}
            string(const std::ostringstream& sst) : String(sst.str().c_str()) {}

            string& operator = (const string& st) { *this = st; return *this; }

            string substr(pos from) const { return string(String::substring(from)); }
            string substr(pos from, pos len) const { return string(String::substring(from, from + len)); }

            int to_int() const { return String::toInt(); }

            bool empty() const { return String::length() == 0; }

            pos find(const char* st) const { return String::indexOf(st); }

            static const pos npos = -1;

            char at(pos pos) const { return String::charAt(pos); }

            bool getline(std::istream& is, char token) { return false; }

            void rtrim() { String::trim(); } // NOTE: should be rtrim
    };

    std::ostream& operator << (std::ostream& out, const string& st);
}

#endif
