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

        return this->resetCascade(it, [&](counters_t::iterator it) {
            while (!(*it)->syncLock(begin, (*it)->value())) {
                if (!(*it)->next()) {
                    ostream out;
                    out << "Can not seek " << (*it)->name() << " (" << "base: " << base << " from: " << begin << ")";
                    throw ParseError(out);
                }
            }
        });
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
