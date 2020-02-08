#include "uICAL/cppstl.h"
#include "uICAL/tzbyid.h"

namespace uICAL {
    TZ::ptr TZbyId::init(const TZIdMap::ptr& tzidmap, const std::string id) {
        return TZ::ptr((TZ*)new TZbyId(tzidmap, id));
    }
            
    TZbyId::TZbyId(const TZIdMap::ptr& tzidmap, const std::string id)
    : tzidmap(tzidmap)
    , id(id)
    {
        this->aware = true;
    }

    seconds_t TZbyId::toUTC(seconds_t timestamp) const {
        return timestamp - (this->tzidmap->getOffset(this->id) * 60);
    }

    seconds_t TZbyId::fromUTC(seconds_t timestamp) const {
        return timestamp + (this->tzidmap->getOffset(this->id) * 60);
    }

    void TZbyId::str(std::ostream& out) const {
        out << this->tzidmap->getName(this->id);
    }
}