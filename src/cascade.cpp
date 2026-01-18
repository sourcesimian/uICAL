/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/util.h"
#include "uICAL/error.h"
#include "uICAL/counter.h"
#include "uICAL/logging.h"
#include "uICAL/cascade.h"

namespace uICAL {
    // Cascade::Cascade() {}

    void Cascade::add(Counter_ptr counter) {
        this->counters.push_back(counter);
    }

    void Cascade::add(std::vector<Counter_ptr> counters) {
        for (Counter_ptr counter : counters) {
            this->counters.push_back(counter);
        }
    }

    size_t Cascade::size() const {
        return this->counters.size();
    }

    bool Cascade::reset(const DateStamp& base) {
        std::ignore = base;  // TODO
        return false;
    }

    DateStamp Cascade::value() const {
        return this->counters.front()->value();
    }

    bool Cascade::syncLock(const DateStamp& from, const DateStamp& now) const {
        std::ignore = from;  // TODO
        std::ignore = now;  // TODO
        return true;
    };

    const string Cascade::name() const {
        Joiner names(',');
        for (auto counter : this->counters) {
            counter->str(names.out());
            names.next();
        }
        ostream out;
        names.write(out);
        return out.str();
    }

    void Cascade::wrap() {
    }

    bool Cascade::initCounters(const DateStamp& base, const DateStamp& begin) {
        counters_t::iterator it = this->counters.end();
        -- it;
        if(!(*it)->reset(base)) {
            return false;
        }

        // Flag to track if any counter exhausted its span while seeking past begin.
        // This happens when DTSTART doesn't fall on a valid recurrence day (e.g.,
        // DTSTART is Friday but BYDAY=TU,TH).
        bool spanExhausted = false;

        bool result = this->resetCascade(it, [&](counters_t::iterator it) {
            while (!(*it)->syncLock(begin, (*it)->value())) {
                if (!(*it)->next()) {
                    spanExhausted = true;
                    return;  // Don't throw; let cascade handle advancement
                }
            }
        });

        if (!result) {
            return false;
        }

        // If a counter exhausted its span, advance the cascade to find the first
        // valid occurrence >= begin. This handles cases like WEEKLY with BYDAY=TU,TH
        // where DTSTART falls on Friday - we need to advance to the next week.
        if (spanExhausted) {
            const int maxIterations = 1000;  // Safety limit
            for (int i = 0; i < maxIterations; ++i) {
                if (!this->next()) {
                    return false;
                }
                if (begin <= this->value()) {
                    return true;
                }
            }
            return false;
        }

        return true;
    }

    bool Cascade::resetCascade(counters_t::iterator it, sync_f sync) {
        DateStamp base = (*it)->value();
        for (;;) {
            if (it == this->counters.begin())
                break;
            -- it;
            if (!(*it)->reset(base)) {
                ++ it;
                if(!(*it)->next()) {
                    ++ it;
                    if (it != this->counters.end()) {
                        throw ImplementationError("No next available from upper level counter");
                    }
                    -- it;
                }
            }

            if (sync != nullptr) {
                sync(it);
            }

            if (it == this->counters.begin())
                break;
            base = (*it)->value();
            // if (this->expired(base)) {
            //     return false;
            // }
        }
        return true;
    }

    bool Cascade::next() {
        counters_t::iterator it = this->counters.begin();

        if(!(*it)->next()) {
            for (;;) {
                ++ it;
                if (it == this->counters.end()) {
                    -- it;
                    if (it == this->counters.begin())
                        return true;
                    break;
                }
                if ((*it)->next())
                    break;
            }
            if (!this->resetCascade(it, nullptr)) {
                return false;
            }
        }
        return true;
    }

    void Cascade::str(ostream& out) const {
        Joiner counters('+');
        for (auto counter : this->counters) {
            counter->str(counters.out());
            counters.next();
        }
        counters.str(out);
    }
}
