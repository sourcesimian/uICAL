#ifndef uical_component_h
#define uical_component_h

#include "uICAL/icalline.h"

namespace uICAL {
    class VComponent {
        public:
            using ptr = std::shared_ptr<VComponent>;
            using vector = std::vector<ptr>;

            VComponent(VLine::ptr beginline);
            static ptr parse(const std::string ical);
            static ptr parse(std::istringstream& ical);
            static ptr parse(VLineReader::ptr& lines);

            VLine::ptr getPropertyByName(std::string name);
            vector listComponents(std::string name);

            std::string getName();

            void str(std::ostream& out) const;

        protected:
            void addLine(VLine::ptr line);
            void addComponent(VComponent::ptr component);

            std::string name;
            std::vector<VLine::ptr> lines;
            std::vector<VComponent::ptr> children;
    };

    std::ostream & operator << (std::ostream &out, const VComponent::ptr &c);
    std::ostream & operator << (std::ostream &out, const VComponent &c);
}
#endif
