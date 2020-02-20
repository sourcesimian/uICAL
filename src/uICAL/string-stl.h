/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_string_stl_h
#define uical_string_stl_h

#include <string>
#include <sstream>

namespace uICAL {
    class string : public std::string {
        public:
            using pos = size_type;

            string() : std::string() {}
            string(const std::string& b) : std::string(b) {}
            string(const char* st) : std::string(st) {}
            string(const std::ostringstream& sst) : std::string(sst.str()) {}

            string& operator = (const string& st) { std::string::operator =(st); return *this; }

            string substr(pos from) const { return string(std::string::substr(from)); }
            string substr(pos from, pos len) const { return string(std::string::substr(from, len)); }

            int to_int() const { return std::atoi(std::string::c_str()); }

            bool empty() const { return std::string::empty(); }

            pos find(const char* st) const { return std::string::find(st); }

            static const pos npos = -1;

            char at(pos pos) const { return std::string::at(pos); }

            bool getline(std::istream& is, char token) { std::ignore = is; std::ignore = token; return false; }

            void rtrim() { _rtrim(*this); }
            static void _rtrim(string& s);
    };

    std::ostream& operator << (std::ostream& out, const string& st);
}

#endif
