/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/stream.h"
#include "uICAL/string.h"

namespace uICAL {
    ostream& ostream::operator <<(const ostream& stm) {
        for (string st : stm.strings) {
            this->strings.push_back(st);
        }
        return *this;
    }

    ostream& ostream::operator <<(const char* st) {
        this->strings.push_back(st);
        return *this;
    }

    ostream& ostream::operator <<(const string& st) {
        this->strings.push_back(st);
        return *this;
    }

    ostream& ostream::operator <<(unsigned int i) {
        this->strings.push_back(string::fmt("%u", i));
        return *this;
    }

    ostream::operator string() const {
        return this->str();
    }

    bool ostream::empty() const {
        return this->strings.size() == 0;
    }

    void ostream::clear() {
        this->strings.clear();
    }

    string ostream::str() const {
        string ret = "";
        for (string st : this->strings) {
            ret += st;
        }
        return ret;
    }

    #ifdef ARDUINO

        istream_String::istream_String(const String& st)
        : st(st)
        , pos(0)
        {}

        char istream_String::peek() const {
            return this->st.charAt(this->pos);
        }

        char istream_String::get() {
            return this->st.charAt(this->pos++);
        }

        bool istream_String::eof() const {
            return this->pos >= this->st.length();
        }

        bool istream_String::readuntil(string& st, char delim) {
            if (this->eof()) {
                return false;
            }

            size_t index = this->st.indexOf(delim, this->pos);
            if (index == -1) {
                st = this->st.substring(this->pos);
                this->pos = this->st.length();
            }
            else {
                st = this->st.substring(this->pos, index);
                this->pos = index + 1;
            }
            return true;
        }

    #else

        istream_stl::istream_stl(std::istream& istm)
        : istm(istm)
        {}

        char istream_stl::peek() const {
            return this->istm.peek();
        }

        char istream_stl::get() {
            return this->istm.get();
        }

        bool istream_stl::eof() const {
            return this->istm.eof();
        }

        bool istream_stl::readuntil(string& st, char delim) {
            if (std::getline(this->istm, st, delim)) {
                return true;
            }
            return false;
        }

    #endif
}
