/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_bysetpos_h
#define uical_bysetpos_h

#include "uICAL/counter.h"

namespace uICAL {
    class DateStamp;

    class BySetPosCounter : protected CounterT<int> {
        public:
            static Counter_ptr init(Counter_ptr counter, const values_t& values);

            virtual bool reset(const DateStamp& base);
            virtual DateStamp value() const;

            virtual bool syncLock(const DateStamp& from, const DateStamp& now) const;
            virtual const string name() const { return "BySetPos"; }

            virtual void str(ostream& out) const;

        protected:
            BySetPosCounter(Counter_ptr counter, const values_t& values);
            virtual ~BySetPosCounter() = default;

            Counter_ptr counter;
            std::vector<DateStamp> results;
    };
}
#endif
