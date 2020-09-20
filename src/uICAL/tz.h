/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_tz_h
#define uical_tz_h

#include "uICAL/base.h"

namespace uICAL {
    class DateStamp;
    class TZMap;

    class TZ : public Base {
        public:
            static TZ_ptr undef();
            static TZ_ptr unaware();

            TZ();
            TZ(bool aware);
            TZ(int offsetMins);
            TZ(const string& tz);
            TZ(const string& tz, const TZMap_ptr& tzmap);

            seconds_t toUTC(seconds_t timestamp) const;
            seconds_t fromUTC(seconds_t timestamp) const;

            bool is_aware() const;
            int offset() const;

            void str(ostream& out) const;

            static int parseOffset(const string& offset);
            static void offsetAsString(ostream& out, int offsetMins);

        protected:
            bool aware;

        private:
            int offsetMins;
            const TZMap_ptr idmap;
            string id;
    };
}
#endif
