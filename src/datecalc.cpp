/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/datecalc.h"

namespace uICAL {
    // Returns number of days since civil 1970-01-01.  Negative values indicate
    // days prior to 1970-01-01.
    // Preconditions:  y-m-d represents a date in the civil (Gregorian) calendar
    //                 m is in [1, 12]
    //                 d is in [1, last_day_of_month(y, m)]
    //                 y is a year after 1970-01-01
    // Source: http://howardhinnant.github.io/date_algorithms.html#days_from_civil
    unsigned days_from_civil(unsigned y, unsigned m, unsigned d) noexcept
    {
        y -= m <= 2;
        const unsigned era = (y >= 0 ? y : y-399) / 400;
        const unsigned yoe = y - era * 400;                             // [0, 399]
        const unsigned doy = (153*(m + (m > 2 ? -3 : 9)) + 2)/5 + d-1;  // [0, 365]
        const unsigned doe = yoe * 365 + yoe/4 - yoe/100 + doy;         // [0, 146096]
        return era * 146097 + doe - 719468;
    }

    // Returns year/month/day triple in civil calendar
    // Preconditions:  z is number of days since 1970-01-01
    // Source: http://howardhinnant.github.io/date_algorithms.html#civil_from_days
    std::tuple<unsigned, unsigned, unsigned> civil_from_days(unsigned z) noexcept
    {
        z += 719468;
        const unsigned era = (z >= 0 ? z : z - 146096) / 146097;
        const unsigned doe = z - era * 146097;                                 // [0, 146096]
        const unsigned yoe = (doe - doe/1460 + doe/36524 - doe/146096) / 365;  // [0, 399]
        const unsigned y = yoe + era * 400;
        const unsigned doy = doe - (365*yoe + yoe/4 - yoe/100);                // [0, 365]
        const unsigned mp = (5*doy + 2)/153;                                   // [0, 11]
        const unsigned d = doy - (153*mp+2)/5 + 1;                             // [1, 31]
        const unsigned m = mp + (mp < 10 ? 3 : -9);                            // [1, 12]
        return std::tuple<unsigned, unsigned, unsigned>(y + (m <= 2), m, d);
    }

    // Returns: true if y is a leap year in the civil calendar, else false
    // Source: http://howardhinnant.github.io/date_algorithms.html#is_leap
    bool is_leap(unsigned y) noexcept
    {
        return  y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
    }

    // Preconditions: m is in [1, 12]
    // Returns: The number of days in the month m of common year
    // The result is always in the range [28, 31].
    // Source: http://howardhinnant.github.io/date_algorithms.html#last_day_of_month_common_year
    unsigned last_day_of_month_common_year(unsigned m) noexcept
    {
        constexpr unsigned char a[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return a[m-1];
    }
    
    // Preconditions: m is in [1, 12]
    // Returns: The number of days in the month m of leap year
    // The result is always in the range [29, 31].
    // Source: http://howardhinnant.github.io/date_algorithms.html#last_day_of_month_leap_year
    unsigned last_day_of_month_leap_year(unsigned m) noexcept
    {
        constexpr unsigned char a[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        return a[m-1];
    }

    // Preconditions: m is in [1, 12]
    // Returns: The number of days in the month m of year y
    // The result is always in the range [28, 31].
    // Source: http://howardhinnant.github.io/date_algorithms.html#last_day_of_month
    unsigned last_day_of_month(unsigned y, unsigned m) noexcept
    {
        return m != 2 || !is_leap(y) ? last_day_of_month_common_year(m) : 29u;
    }

    // Returns day of week in civil calendar [0, 6] -> [Sun, Sat]
    // Preconditions:  z is number of days since 1970-01-01
    // Source: http://howardhinnant.github.io/date_algorithms.html#weekday_from_days
    unsigned weekday_from_days(unsigned z) noexcept
    {
        return (z+4) % 7;
    }

    dhms_t to_dhms(seconds_t seconds) {
        unsigned hour, minute, second;

        second = seconds % 60;
        seconds /= 60;

        minute = seconds % 60;
        seconds /= 60;

        hour = seconds % 24;
        seconds /= 24;

        return dhms_t(seconds, hour, minute, second);
    }

    seconds_t to_seconds(unsigned day, unsigned hour, unsigned minute, unsigned second) {
        return (seconds_t)day * 60 * 60 * 24 + hour * 60 * 60 + minute * 60 + second;
    }
}
