/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/calendarentry.h"

namespace uICAL {
    CalendarEntry::ptr CalendarEntry::init(Type type, const string& summary, const DateTime& start) {
        return CalendarEntry::ptr(new CalendarEntry(type, summary, start));
    }

    CalendarEntry::ptr CalendarEntry::init(Type type, const string& summary, const DateTime& start, const DatePeriod& span) {
        return CalendarEntry::ptr(new CalendarEntry(type, summary, start, span));
    }

    CalendarEntry::CalendarEntry(Type type, const string& summary, const DateTime& start) {
        this->_type = type;
        this->_summary = summary;
        this->_start = start;
    }

    CalendarEntry::CalendarEntry(Type type, const string& summary, const DateTime& start, const DatePeriod& span) {
        this->_type = type;
        this->_summary = summary;
        this->_start = start;
        this->_span = span;
    }

    CalendarEntry::Type CalendarEntry::type() const {
        return this->_type;
    }

    string CalendarEntry::asString(Type type) {
        switch(type) {
            case Type::EVENT:
                return "EVENT";
            case Type::NONE:
            default:
                return "UNKNOWN";
        }
    }

    const string CalendarEntry::summary() const {
        return this->_summary;
    }

    DateTime CalendarEntry::start() const {
        return this->_start;
    }

    DateTime CalendarEntry::end() const {
        return this->_start + this->_span;
    }

    void CalendarEntry::str(ostream& out) const {
        out << "Calendar " << this->asString(this->_type) << ": " << this->_summary << uICAL::endl;
        out << " - start: " << this->_start << uICAL::endl;
        out << " - span: " << this->_span << uICAL::endl;
    }

    ostream& operator << (ostream& out, const CalendarEntry::ptr& ce) {
        ce->str(out);
        return out;
    }
}
