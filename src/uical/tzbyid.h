#ifndef uical_tzbyid_h
#define uical_tzbyid_h

#include "uICAL/types.h"
#include "uICAL/tz.h"
#include "uICAL/tzidmap.h"


namespace uICAL {
    class TZbyId : public TZ {
        public:
            static TZ::ptr init(const TZIdMap::ptr& tzidmap, const std::string id);            
            TZbyId(const TZIdMap::ptr& tzidmap, const std::string id);

            virtual seconds_t toUTC(seconds_t timestamp) const;
            virtual seconds_t fromUTC(seconds_t timestamp) const;

            void str(std::ostream& out) const;
        
        protected:
            const TZIdMap::ptr tzidmap;
            const std::string id;
    };
}
#endif
