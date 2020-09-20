/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
//#include <cstdlib>

#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"

namespace uICAL {
    Joiner::Joiner(char delim) {
        this->delim = delim;
    }

    ostream& Joiner::out() {
        return this->stm;
    }

    Joiner& Joiner::next() {
        if (this->stm.empty()) {
            this->values.push_back("");
        }
        else {
            this->values.push_back(this->stm);
            this->stm.clear();
        }
        return *this;
    }

    void Joiner::str(ostream& out) const {
        bool started = false;
        for (string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }

    void Joiner::write(ostream& out) const {
        bool started = false;
        for (string value : this->values) {
            if (started) out << this->delim;
            else started = true;
            out << value;
        }
    }
}
