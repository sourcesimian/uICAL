/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include <cstdlib>

#include "uICAL/cppstl.h"
#include "uICAL/util.h"

namespace uICAL {
    void tokenize(const std::string& input, char token, std::function<void (std::string)> cb) {
        std::istringstream stream(input);
        std::string part;
        while(std::getline(stream, part, token)) {
            cb(part);
        }
    }

    int string_to_int(const std::string& input) {
        return atoi(input.c_str());
    }

    Joiner::Joiner(char delim) {
        this->delim = delim;
    }

    std::ostringstream& Joiner::out() {
        return this->current;
    }

    Joiner& Joiner::next() {
        this->values.push_back(this->current.str());
        this->current.str("");
        return *this;
    }

    std::string Joiner::str() const {
        std::ostringstream ss;
        this->write(ss);
        return ss.str();
    }

    void Joiner::str(std::ostream& out) const {
        bool started = false;
        for (std::string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }

    void Joiner::write(std::ostream& out) const {
        bool started = false;
        for (std::string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }

    void ltrim(std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
            return !std::isspace(ch);
        }));
    }

    void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    }
}
