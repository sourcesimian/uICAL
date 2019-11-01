#include "uICAL/cppstl.h"
#include "uICAL/error.h"
#include "uICAL/icalcomponent.h"
#include "uICAL/util.h"

namespace uICAL {

    VComponent::VComponent(VLine::ptr beginline) {
        this->name = beginline->value;
    }

    std::string VComponent::getName() {
        return this->name;
    }

    void VComponent::addLine(VLine::ptr line) {
        this->lines.push_back(line);
    }

    void VComponent::addComponent(VComponent::ptr component) {
        this->children.push_back(component);
    }

    VComponent::ptr VComponent::parse(const std::string ical) {
        std::istringstream icals(ical);
        VComponent::ptr comp = VComponent::parse(icals);
        return comp;
    }

    VComponent::ptr VComponent::parse(std::istringstream& ical) {
        VLineReader::ptr lines = std::make_shared<VLineReaderStream>(ical);
        VComponent::ptr comp = VComponent::parse(lines);
        return comp;
    }

    VComponent::ptr VComponent::parse(VLineReader::ptr& lines) {
        VLine::ptr line = lines->peek();

        if (line->name != "BEGIN") {
            throw ParseError(std::string("BEGIN not found in: ???"));  // TODO
        }        
        std::string section = line->value;
        lines->pop();

        VComponent::ptr component = std::make_shared<VComponent>(line);

        try {
            while(true) {
                line = lines->peek();

                if (line->name == "END" && section == section) {
                    lines->pop();
                    return component;
                }

                if (line->name == "BEGIN") {
                    component->addComponent(parse(lines));
                }
                else
                {
                    component->addLine(line);
                    lines->pop();
                }
            }
        }
        catch (VLineReaderEnd e) {
            return component;
        }
    }

    VLine::ptr VComponent::getPropertyByName(std::string name) {
        for (auto it = this->lines.begin(); it != this->lines.end(); ++it) {
            if ( (*it)->name == name) {
                return (*it);
            }
        }
        return VLine::init(std::string(""));
    }

    VComponent::vector VComponent::listComponents(std::string name) {
        VComponent::vector ret;
        for ( auto it = this->children.begin(); it != this->children.end(); ++it) {
            if ((*it)->name == name) {
                ret.push_back(*it);
            }
        }
        return ret;
    }

    std::ostream & operator << (std::ostream &out, const VComponent::ptr &c) {
        c->str(out);
        return out;
    }

    std::ostream & operator << (std::ostream &out, const VComponent &c) {
        c.str(out);
        return out;
    }
    
    void VComponent::str(std::ostream& out) const {
        out << "BEGIN:" << this->name << std::endl;
        for (auto line : this->lines) {
            out << line << std::endl;
        }

        for (auto child : this->children) {
            out << child;
        }

        out << "END:" << this->name << std::endl;
    }
}