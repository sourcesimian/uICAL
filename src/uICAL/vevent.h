/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vevent_h
#define uical_vevent_h

#include "uICAL/base.h"
#include "uICAL/datetime.h"

namespace uICAL {
    class VObject;

    class VEvent : public Base {
        public:
            VEvent(const VObject_ptr& event, const TZMap_ptr& tzmap);

            void str(ostream& out) const;

        private:

            DateTime start;
            DateTime end;
            string summary;
            RRule_ptr rrule;

            friend class VEventIter;
    };
}
#endif
