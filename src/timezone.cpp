#include "uICAL/cppstl.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/timezone.h"

namespace uICAL {
    Timezones::Timezones(VComponent& calendar) {
        auto timezones = calendar.listComponents("VTIMEZONE");
        for (auto it = timezones.begin(); it != timezones.end(); ++it) {
            std::string tzId = (*it)->getPropertyByName("TZID")->value;

            auto standards = (*it)->listComponents("STANDARD");
            for (auto it = standards.begin(); it != standards.end(); ++it) {
                std::string offset = (*it)->getPropertyByName("TZOFFSETFROM")->value;
                this->timezone_map[tzId] = offset;
            }
        }
    }

    std::string Timezones::getOffset(const std::string tzId) {
        return this->timezone_map[tzId]; // TODO: handle KeyError
    }
}
