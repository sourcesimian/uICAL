#include "uICAL/cppstl.h"
#include "uICAL/util.h"
#include "uICAL/byandcounter.h"
#include "uICAL/error.h"

namespace uICAL {
    Counter::ptr ByAndCounter::init(std::vector<Counter::ptr> counters) {
        if (counters.size() < 2) {
            return counters.front();
        }
        return Counter::ptr((Counter*)new ByAndCounter(counters));
    }

    ByAndCounter::ByAndCounter(std::vector<Counter::ptr> counters) 
    : counters(counters)
    {
        if (this->counters.size() < 2) {
            throw ImplementationError("ByAndCounter only for more than 2 ByCounters, use newAnd()");
        }
    }

    void ByAndCounter::wrap() {
        this->current = nullptr;
    }

    bool ByAndCounter::syncLock(DateStamp from, DateStamp now) const {
        for(auto counter : this->counters) {
            if  (!counter->syncLock(from, now))
                return false;
        }
        return true;
    }

    bool ByAndCounter::reset(DateStamp base) {
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
            Counter::ptr min = *std::min_element(this->counters.begin(), this->counters.end());

            bool notEqual = false;
            for (auto counter : this->counters) {
                if (min != counter) {
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
            
    void ByAndCounter::str(std::ostream& out) const {
        Joiner values(',');
        for (auto counter : this->counters) {
            counter->str(values.out());
            values.next();
        }
        out << "["; values.str(out); out << "]";
    }
}