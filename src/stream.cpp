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

    ostream& ostream::operator <<(char ch) {
        this->strings.push_back(string::fmt("%c", ch));
        return *this;
    }

    ostream& ostream::operator <<(int i) {
        this->strings.push_back(string::fmt("%d", i));
        return *this;
    }

    ostream& ostream::operator <<(unsigned int i) {
        this->strings.push_back(string::fmt("%u", i));
        return *this;
    }

    ostream& ostream::operator <<(long long int i) {
        this->strings.push_back(string::fmt("%lld", i));
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

        istream_Stream::istream_Stream(Stream& stm)
        : stm(stm)
        {}

        char istream_Stream::peek() const {
            int ch = this->stm.peek();
            return ch;
        }

        char istream_Stream::get() {
            int ch = this->stm.read();
            return ch;
        }

        bool istream_Stream::readuntil(string& st, char delim) {
            size_t len = 81;
            char buf[len];

            size_t read = this->stm.readBytesUntil(delim, buf, len-1);
            if (read > 0) {
                buf[read] = 0;
                st = buf;
                return true;
            }
            return false;
        }

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

        bool istream_String::readuntil(string& st, char delim) {
            if (this->pos >= this->st.length()) {
                return false;
            }

            size_t index = this->st.indexOf(delim, this->pos);
            if (index == (size_t)-1) {
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

        bool istream_stl::readuntil(string& st, char delim) {
            if (std::getline(this->istm, st, delim)) {
                return true;
            }
            return false;
        }

    #endif
}
