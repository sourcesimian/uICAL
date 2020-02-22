/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_component_h
#define uical_component_h

#include "uICAL/base.h"
#include "uICAL/icalline.h"

namespace uICAL {
    class VComponent : public Base {
        public:
            using ptr = std::shared_ptr<VComponent>;
            using vector = std::vector<ptr>;

            VComponent(VLine::ptr beginline);

            static ptr parse(istream& ical);
            static ptr parse(VLineReader::ptr& lines);

            VLine::ptr getPropertyByName(const string& name);
            vector listComponents(const string& name);

            string getName();

            void str(ostream& out) const;

        protected:
            void addLine(VLine::ptr line);
            void addComponent(VComponent::ptr component);

            string name;
            std::vector<VLine::ptr> lines;
            std::vector<VComponent::ptr> children;
    };

    ostream& operator << (ostream& out, const VComponent::ptr& c);
    ostream& operator << (ostream& out, const VComponent& c);
}
#endif
