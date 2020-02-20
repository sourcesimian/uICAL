/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <cstdlib>

#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"

namespace uICAL {
    void tokenize(const string& input, char token, std::function<void (string)> cb) {
        std::istringstream stream(input.c_str());
        string part;
        while(part.getline(stream, token)) {
            cb(part);
        }
    }

    Joiner::Joiner(char delim) {
        this->delim = delim;
    }

    std::ostringstream& Joiner::out() {
        return this->current;
    }

    Joiner& Joiner::next() {
        this->values.push_back(this->current);
        this->current.str("");
        return *this;
    }

    string Joiner::str() const {
        std::ostringstream ss;
        this->write(ss);
        return ss;
    }

    void Joiner::str(std::ostream& out) const {
        bool started = false;
        for (string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }

    void Joiner::write(std::ostream& out) const {
        bool started = false;
        for (string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }

    // void ltrim(string& s) {
    //     s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
    //         return !std::isspace(ch);
    //     }));
    // }

    // void rtrim(string& s) {
    //     s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
    //         return !std::isspace(ch);
    //     }).base(), s.end());
    // }
}
