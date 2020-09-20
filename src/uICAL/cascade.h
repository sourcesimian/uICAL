/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_cascade_h
#define uical_cascade_h

#include "uICAL/types.h"
#include "uICAL/counter.h"

namespace uICAL {

    class Cascade : public Counter {
        public:
            using counters_t = std::vector<Counter_ptr>;
            using sync_f = std::function<void(counters_t::iterator it)>;

            Cascade() {}
            virtual ~Cascade() = default;

            virtual bool reset(const DateStamp& base);
            virtual bool next();
            virtual DateStamp value() const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const;

            virtual void str(ostream& out) const;

            void add(Counter_ptr counter);
            void add(std::vector<Counter_ptr> counters);
            size_t size() const;

            void setupCounters(const DateStamp& base);
            bool resetCascade(counters_t::iterator it, sync_f sync);
            bool initCounters(const DateStamp& base, const DateStamp& from);

        protected:
            virtual void wrap();

            counters_t counters;
    };
}
#endif
