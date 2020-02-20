/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_stl_h
#define uical_string_stl_h

#include <cstdlib>

#include <string>
#include <sstream>
#include <iomanip>

namespace uICAL {
    using std::endl;

    class string : public std::string {
        public:

            string() : std::string() {}
            string(const std::string& b) : std::string(b) {}
            string(const char* st) : std::string(st) {}
            string(const std::ostringstream& sst) : std::string(sst.str()) {}

            string& operator = (const string& st) { std::string::operator =(st); return *this; }
    };

    // using ostream = std::ostringstream;
    // class ostream : public std::ostringstream {
    // };

    std::ostream& operator << (std::ostream& out, const string& st);

    int to_int(const string&);
    bool getline(std::istream& is, char token, string& st);
    void rtrim(string& st);
}

#endif
