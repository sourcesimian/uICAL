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
        while(getline(stream, part, token)) {
            cb(part);
        }
    }

    Joiner::Joiner(char delim) {
        this->delim = delim;
    }

    std::ostringstream& Joiner::out() {
        return this->stream;
    }

    Joiner& Joiner::next() {
        if (this->stream.tellp() == 0) {
            this->values.push_back(this->stream.str().c_str());
            this->stream.seekp(0);
        }
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
}
