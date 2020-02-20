/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_arduino_h
#define uical_string_arduino_h

#include <WString.h>

#include <string>
#include <sstream>
#include <iomanip>

namespace uICAL {
    extern const char* endl;

    class string : public String {
        public:
            using size_t = unsigned int;
            static const size_t npos = -1;
            
            string() : String() {}
            string(const String& b) : String(b) {}
            string(const char* st) : String(st) {}
            string(const StringSumHelper& ssh) : String(ssh) {}
            string(const std::ostringstream& sst) : String(sst.str().c_str()) {}

            string substr(size_t from) const { return string(String::substring(from)); }
            string substr(size_t from, size_t len) const { return string(String::substring(from, from + len)); }

            bool empty() const { return String::length() == 0; }

            size_t find(const char* st) const { return String::indexOf(st); }

            char at(size_t pos) const { return String::charAt(pos); }

            void clear() { *this = ""; }

            friend int to_int(const string&);
            friend bool getline(std::istream& is, char token, string& st);
            friend void rtrim(string& st);

        // protected:
        //     String s;
    };

    std::ostream& operator << (std::ostream& out, const string& st);

    int to_int(const string&);
    bool getline(std::istream& is, string& st, char token);
    void rtrim(string& st);
}

#endif
