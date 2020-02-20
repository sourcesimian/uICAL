/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef ARDUINO

#include "uICAL/cppstl.h"
#include "uICAL/string.h"
#include "uICAL/stream.h"

namespace uICAL {
    stream& stream::operator <<(const stream& stm) {
        for (string st : stm.strings) {
            this->strings.push_back(st);
        }
        return *this;
    }

    stream& stream::operator <<(const char* st) {
        this->strings.push_back(st);
        return *this;
    }

    stream& stream::operator <<(const string& st) {
        this->strings.push_back(st);
        return *this;
    }

    string stream::str() const {
        string ret = "";
        for (string st : this->strings) {
            ret += st;
        }
        return ret;
    }
}

#endif
