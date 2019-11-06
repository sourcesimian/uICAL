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
        this->offsetMins = this->tzidmap->getOffset(this->id);
    }

    void TZbyId::str(std::ostream& out) const {

    }
}