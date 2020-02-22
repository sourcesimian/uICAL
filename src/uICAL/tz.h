/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_tz_h
#define uical_tz_h

#include "uICAL/base.h"
#include "uICAL/tzmap.h"

namespace uICAL {
    class DateStamp;

    class TZ : public Base {
        public:
            using ptr = std::shared_ptr<TZ>;

            static ptr init(int offsetMins);
            static ptr init(const string& tz);
            static ptr init(const string& tz, const TZMap::ptr& tzmap);

            static ptr undef();
            static ptr unaware();

            TZ();
            TZ(bool aware);
            TZ(int offsetMins);
            TZ(const string& tz);
            TZ(const string& tz, const TZMap::ptr& tzmap);

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
            const TZMap::ptr idmap;
            string id;
    };
    
    ostream& operator << (ostream& out, const TZ::ptr& tz);
}
#endif
