/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_types_h
#define uical_types_h

#include "uICAL/stream.h"
#include "uICAL/string.h"

namespace uICAL {
    using seconds_t = long long int;

    template<typename T, typename... Args> \
    inline std::shared_ptr<T> new_ptr(Args... args) { return std::shared_ptr<T>(new T(args...)); }

    #define UICAL_PTR(T) \
        class T; \
        using T##_ptr = std::shared_ptr<T>;

    UICAL_PTR(Calendar)
    UICAL_PTR(CalendarIter)
    UICAL_PTR(CalendarEntry)
    UICAL_PTR(Counter)
    UICAL_PTR(DateTime)
    UICAL_PTR(RRule)
    UICAL_PTR(RRuleIter)
    UICAL_PTR(TZ)
    UICAL_PTR(TZMap)
    UICAL_PTR(VComponent)
    UICAL_PTR(VEvent)
    UICAL_PTR(VEventIter)
    UICAL_PTR(VLine)
    UICAL_PTR(VObject)

    #undef UICAL_PTR
}
#endif
