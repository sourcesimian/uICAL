/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/bysetposcounter.h"

namespace uICAL {
    Counter::ptr BySetPosCounter::init(Counter::ptr counter, const values_t& values) {
        if (values.size()) {
            return Counter::ptr((Counter*)new BySetPosCounter(counter, values));
        }
        return counter;
    }

    BySetPosCounter::BySetPosCounter(Counter::ptr counter, const values_t& values)
    : CounterT(values), counter(counter)
    {}

    bool BySetPosCounter::reset(const DateStamp& base) {
        if (!this->counter->reset(base)) {
            return false;
        }

        this->results.clear();
        do {
            DateStamp ds = this->counter->value();
            this->results.push_back(ds);
        } while (this->counter->next());
        return CounterT::reset(base);
    }

    DateStamp BySetPosCounter::value() const {
        if (*this->it > 0) {
            return this->results[(*this->it) - 1];
        }
        else {
            return this->results[this->results.size() + (*this->it)];
        }
    }

    bool BySetPosCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        return this->counter->syncLock(from, now);
    }
    
    void BySetPosCounter::str(std::ostream& out) const {
        this->counter->str(out);
        out << "[" << this->values << "]";
    }

}