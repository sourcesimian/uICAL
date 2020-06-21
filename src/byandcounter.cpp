/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/util.h"
#include "uICAL/byandcounter.h"

namespace uICAL {
    Counter_ptr ByAndCounter::init(std::vector<Counter_ptr> counters) {
        if (counters.size() < 2) {
            return counters.front();
        }
        return Counter_ptr((Counter*)new ByAndCounter(counters));
    }

    ByAndCounter::ByAndCounter(std::vector<Counter_ptr> counters)
    : counters(counters)
    {
        if (this->counters.size() < 2) {
            throw ImplementationError("ByAndCounter only for more than 2 ByCounters, use newAnd()");
        }
    }

    void ByAndCounter::wrap() {
        this->current = nullptr;
    }

    bool ByAndCounter::syncLock(const DateStamp& from, const DateStamp& now) const {
        for(auto counter : this->counters) {
            if  (!counter->syncLock(from, now))
                return false;
        }
        return true;
    }

    bool ByAndCounter::reset(const DateStamp& base) {
        for(auto counter : this->counters) {
            counter->reset(base);
        }
        this->current = nullptr;
        if(!this->findNextCommon()) {
            return false;
        }
        return true;
    }

    bool ByAndCounter::next() {
        return this->findNextCommon();
    }

    bool ByAndCounter::findNextCommon() {
        if (this->current != nullptr) {
            if (!this->current->next()) {
                this->wrap();
                return false;
            }
        }
        for (;;) {
            Counter_ptr min = *std::min_element(this->counters.begin(), this->counters.end());
            bool notEqual = false;
            for (auto counter : this->counters) {
                if (min->value() != counter->value()) {
                    notEqual = true;
                    break;
                }
            }
            if (notEqual) {
                if(!min->next()) {
                    this->wrap();
                    return false;
                }
                continue;
            }
            this->current = min;
            return true;
        }
    }

    DateStamp ByAndCounter::value() const {
        return this->current->value();
    }

    void ByAndCounter::str(ostream& out) const {
        Joiner values('&');
        for (auto counter : this->counters) {
            counter->str(values.out());
            values.next();
        }
        out << "("; values.str(out); out << ")";
    }
}
