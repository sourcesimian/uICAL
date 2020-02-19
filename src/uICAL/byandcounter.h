/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_bycounterset_h
#define uical_bycounterset_h

#include "uICAL/counter.h"

namespace uICAL {
    class ByAndCounter : public Counter {
        public:
            static Counter::ptr init(std::vector<Counter::ptr> counters);

            virtual bool next();
            virtual bool reset(const DateStamp& base);

            virtual DateStamp value() const;

            virtual void str(std::ostream& out) const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;

        protected:
            ByAndCounter(std::vector<Counter::ptr> counters);
            virtual ~ByAndCounter() {};

            virtual const string name() const { return "ByAndCounter"; };
            virtual void wrap();

            bool findNextCommon();

            std::vector<Counter::ptr> counters;
            Counter::ptr current;
    };
}
#endif
