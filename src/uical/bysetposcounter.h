#ifndef uical_bysetpos_h
#define uical_bysetpos_h

#include "uICAL/counter.h"
#include "uICAL/datestamp.h"

namespace uICAL {
   class BySetPosCounter : protected CounterT<int> {
        public:
            static Counter::ptr init(Counter::ptr counter, const values_t& values);

            virtual bool reset(DateStamp base);
            virtual DateStamp value() const;

            virtual bool syncLock(DateStamp from, DateStamp now) const;
            virtual const std::string name() const { return "BySetPos"; }

            virtual void str(std::ostream& out) const;

        protected:
            BySetPosCounter(Counter::ptr counter, const values_t& values);

            Counter::ptr counter;
            std::vector<DateStamp> results;
    };
}
#endif
