#include "uICAL/cppstl.h"
#include "uICAL/entry.h"

namespace uICAL {
    Entry::Entry(Type type, std::string summary, DateTime start) {
        this->_type = type;
        this->_summary = summary;
        this->_start = start;
    }

    Entry::Entry(Type type, std::string summary, DateTime start, DateSpan span) {
        this->_type = type;
        this->_summary = summary;
        this->_start = start;
        this->_span = span;
    }

    Entry::Type Entry::type() const {
        return this->_type;
    }

    std::string Entry::asString(Type type) {
        switch(type) {
            case Type::EVENT:
                return "EVENT";
            case Type::NONE:
            default:
                return "UNKNOWN";
        }
    }

    const std::string Entry::summary() const {
        return this->_summary;
    }

    DateTime Entry::start() const {
        return this->_start;
    }

    DateSpan Entry::span() const {
        return this->_span;
    }
}
