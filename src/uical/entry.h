#ifndef uical_entry_h
#define uical_entry_h

#include "uICAL/datetime.h"
#include "uICAL/datespan.h"

namespace uICAL {
    class Entry {
        public:
            enum class Type {
                NONE, EVENT,
            };

            Entry(Type type, std::string summary, DateTime start);
            Entry(Type type, std::string summary, DateTime start, DateSpan span);
            
            Type type() const;
            const std::string summary() const;
            DateTime start() const;
            DateSpan span() const;

            static std::string asString(Type type);

        protected:
            Type _type;
            std::string _summary;
            DateTime _start;
            DateSpan _span;
    };
}

#endif