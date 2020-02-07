#ifndef uical_datecalc_h
#define uical_datecalc_h

namespace uICAL {
    unsigned days_from_civil(unsigned y, unsigned m, unsigned d) noexcept;
    std::tuple<unsigned, unsigned, unsigned> civil_from_days(unsigned z) noexcept;
    bool is_leap(unsigned y) noexcept;
    unsigned last_day_of_month_common_year(unsigned m) noexcept;
    unsigned last_day_of_month_leap_year(unsigned m) noexcept;
    unsigned last_day_of_month(unsigned y, unsigned m) noexcept;
    unsigned weekday_from_days(unsigned z) noexcept;
    unsigned weekday_difference(unsigned x, unsigned y) noexcept;
    unsigned next_weekday(unsigned wd) noexcept;
    unsigned prev_weekday(unsigned wd) noexcept;

    using dhms_t = std::tuple<unsigned, unsigned, unsigned, unsigned>;

    dhms_t to_dhms(seconds_t seconds);
    seconds_t to_seconds(unsigned day, unsigned hour, unsigned minute, unsigned second);
}
#endif
