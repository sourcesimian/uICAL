/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_types_h
#define uical_types_h

#include "uICAL/stream.h"
#include "uICAL/string.h"

namespace uICAL {
    using seconds_t = long long int;

    /**
     * Invalid date handling mode per RFC 5545 / RFC 7529.
     *
     * When a recurrence rule generates an invalid date (e.g., Feb 30, or Feb 29
     * in a non-leap year), this setting controls the behavior:
     *
     * - SKIP: Invalid dates are omitted entirely (RFC 5545 compliant, default).
     *         Libraries like rrule.js, python-dateutil use this behavior.
     *
     * - BACKWARD: Invalid dates are moved backward to the nearest valid date.
     *             Consumer apps like Google Calendar, Apple Calendar, Outlook
     *             use this behavior for better user experience.
     *
     * Example: FREQ=YEARLY on Feb 29 (leap day birthday)
     * - SKIP: No occurrence in non-leap years
     * - BACKWARD: Occurs on Feb 28 in non-leap years
     */
    enum class InvalidDateMode {
        SKIP,      // RFC 5545: ignore invalid dates (default)
        BACKWARD,  // RFC 7529: move to previous valid date
    };

    // Global setting for invalid date handling (default: SKIP for RFC compliance)
    InvalidDateMode getInvalidDateMode();
    void setInvalidDateMode(InvalidDateMode mode);

    template<typename T, typename... Args> \
    inline std::shared_ptr<T> new_ptr(Args... args) { return std::shared_ptr<T>(new T(args...)); }

    #define UICAL_PTR(T) \
        class T; \
        using T##_ptr = std::shared_ptr<T>;

    UICAL_PTR(Calendar)
    UICAL_PTR(CalendarIter)
    UICAL_PTR(CalendarEntry)
    UICAL_PTR(Cascade)
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
