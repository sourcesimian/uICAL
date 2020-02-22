/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/stream.h"
#include "uICAL/string.h"

//#include <iostream>

namespace uICAL {
    const char* endl = "\n";
    const char* fmt_04d = "%04d";
    const char* fmt_02d = "%02d";

    void string::tokenize(char delim, std::function<void (string)> cb) const {
        if (this->empty()) {
            return;
        }
        char pattern[2];
        pattern[0] = delim;
        pattern[1] = 0;
        size_t cursor = 0;
        while(true) {
            size_t index = this->find(pattern, cursor);
            if (index == npos) {
                cb(this->substr(cursor));
                return;
            }
            cb(this->substr(cursor, index - cursor));
            cursor = index + 1;
        }
    }

    bool string::readtoken(istream& is, char delim) {
        return is.readuntil(*this, delim);
    }

    #ifdef ARDUINO

        int string::as_int() const {
            return this->toInt();
        }

        void string::rtrim() {
            this->trim();
        }

    #else

        int string::as_int() const
        {
            return std::atoi(this->c_str());
        }

        void string::rtrim() {
            this->erase(std::find_if(this->rbegin(), this->rend(), [](int ch) {
                return !std::isspace(ch);
            }).base(), this->end());
        }

    #endif
}