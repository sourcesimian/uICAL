/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#include "uICAL/cppstl.h"
#include "uICAL/types.h"
#include "uICAL/error.h"
#include "uICAL/rrule.h"
#include "uICAL/util.h"
#include "uICAL/debug.h"

namespace uICAL {
    RRule::ptr RRule::init(const string& rrule, const DateTime& dtstart) {
        return RRule::ptr(new RRule(rrule, dtstart));
    }

    RRule::RRule(const string& rrule, const DateTime& dtstart)
    : dtstart(dtstart)
    {
        this->freq = Freq::NONE;
        this->wkst = DateTime::Day::MON;
        this->interval = 1;
        this->count = -1;

        if (rrule.empty())
            this->parseRRule("FREQ=DAILY;COUNT=1");
        else
            this->parseRRule(rrule);

        debug(string("RRULE ") + this->str());
    }

    void RRule::parseRRule(const string& rrule) {
        tokenize(rrule, ';', [&](const string part){
            size_t equals = part.find("=");
            const string key = part.substr(0, equals);
            const string value = part.substr(equals + 1, string::npos);

            if (key == "FREQ") {
                if      (value == "SECONDLY") this->freq = Freq::SECONDLY;
                else if (value == "MINUTELY") this->freq = Freq::MINUTELY;
                else if (value == "HOURLY") this->freq = Freq::HOURLY;
                else if (value == "DAILY") this->freq = Freq::DAILY;
                else if (value == "WEEKLY") this->freq = Freq::WEEKLY;
                else if (value == "MONTHLY") this->freq = Freq::MONTHLY;
                else if (value == "YEARLY") this->freq = Freq::YEARLY;
                else {
                    throw ParseError(string("Unknonwn RRULE:FREQ type: ") + value);
                }
            }
            else if (key == "WKST") {
                this->wkst = this->parseDay(value);
            }
            else if (key == "INTERVAL") {
                this->interval = this->parseInt(value);
            }
            else if (key == "UNTIL") {
                this->until = this->parseDate(value);
            }
            else if (key == "COUNT") {
                this->count = this->parseInt(value);
            }
            else if (key == "BYSECOND") {
                this->bySecond = toVector<unsigned>(value);
            }
            else if (key == "BYMINUTE") {
                this->byMinute = toVector<unsigned>(value);
            }
            else if (key == "BYHOUR") {
                this->byHour = toVector<unsigned>(value);
            }
            else if (key == "BYDAY") { // FREQ=MONTHLY;INTERVAL=2;COUNT=10;BYDAY=1SU,-1SU
                this->byDay = this->parseByDay(value);
            }
            else if (key == "BYWEEKNO") {
                this->byWeekNo = toVector<unsigned>(value);
            }
            else if (key == "BYMONTH") {
                this->byMonth = toVector<unsigned>(value);
            }
            else if (key == "BYMONTHDAY") {
                this->byMonthDay = toVector<int>(value);
            }
            else if (key == "BYYEARDAY") {
                this->byYearDay = toVector<int>(value);
            }
            else if (key == "BYSETPOS") {
                this->bySetPos = toVector<int>(value);
            }
            else {
                throw ParseError(string("Unknonwn RRULE key: ") + key);
            }
        });
    }

    int RRule::parseInt(const string& value) const {
        return to_int(value);
    }

    std::vector<string> RRule::parseArray(const string& value) const {
        std::vector<string> array;
        tokenize(value, ',', [&](const string part){
            array.push_back(part);
        });
        return array;
    }

    RRule::Day_vector RRule::parseByDay(const string& value) const {
        Day_vector array;
        tokenize(value, ',', [&](const string part){
            int index;
            if (part.length() > 2) {
                index = to_int(part.substr(0, part.length() - 2));
            }
            else{
                index = 0;
            }

            DateTime::Day day = this->parseDay(part.substr(part.length() - 2, string::npos));
            array.push_back(Day_pair(index, day));
        });
        return array;
    }

    DateTime::Day RRule::parseDay(const string& value) const {
        if (value == "SU") return DateTime::Day::SUN;
        if (value == "MO") return DateTime::Day::MON;
        if (value == "TU") return DateTime::Day::TUE;
        if (value == "WE") return DateTime::Day::WED;
        if (value == "TH") return DateTime::Day::THU;
        if (value == "FR") return DateTime::Day::FRI;
        if (value == "SA") return DateTime::Day::SAT;
        throw ParseError(string("Unknown day name: ") + value);
    }

    DateTime RRule::parseDate(const string& value) const {
        return DateTime(value);
    }

    const char* RRule::dayAsString(DateTime::Day day) const {
        switch(day) {
            case DateTime::Day::MON:    return "MO";
            case DateTime::Day::TUE:    return "TU";
            case DateTime::Day::WED:    return "WE";
            case DateTime::Day::THU:    return "TH";
            case DateTime::Day::FRI:    return "FR";
            case DateTime::Day::SAT:    return "SA";
            case DateTime::Day::SUN:    return "SU";
            default:
                string err("Unknown day index: ");
                err += (int)day;
                throw ParseError(err);
        }
    }

    const char* RRule::frequencyAsString(Freq freq) const {
        switch(freq) {
            case Freq::SECONDLY:  return "SECONDLY";
            case Freq::MINUTELY:  return "MINUTELY";
            case Freq::HOURLY:    return "HOURLY";
            case Freq::DAILY:     return "DAILY";
            case Freq::WEEKLY:    return "WEELKY";
            case Freq::MONTHLY:   return "MONTHLY";
            case Freq::YEARLY:    return "WEEKLY";
            default:
                string err("Unknown frequency index: ");
                err += (int)freq;
                throw ParseError(err);
        }
    }

    string RRule::intAsString(int value) const {
        std::ostringstream out;
        out << value;
        return out;
    }

    void RRule::exclude(const DateTime& exclude) {
        this->excludes.push_back(exclude);
    }

    bool RRule::excluded(const DateTime& now) const {
        auto it = std::find(this->excludes.begin(), this->excludes.end(), now);
        if (it == this->excludes.end()) {
            return false;
        }
        return true;
    }

    string RRule::str() const {
        std::ostringstream out;
        this->str(out);
        return out;
    }

    void RRule::str(std::ostream& out) const {
        out << "RRULE:";

        Joiner values(';');
        
        values.out() << "FREQ=" << + this->frequencyAsString(this->freq);
        values.next();

        if (this->interval) {
            values.out() << "INTERVAL=" << this->intAsString(this->interval);
            values.next();
        }

        if (this->count > 0) {
            values.out() << "COUNT=" << this->intAsString(this->count);
            values.next();
        }

        values.out() << "WKST=" << this->dayAsString(this->wkst);
        values.next();

        if (this->byDay.size()) {
            Joiner days(',');
            for (Day_pair id : this->byDay) {
                if (id.first)
                    days.out() << id.first;
                days.out() << this->dayAsString(id.second);
                days.next();
            }

            values.out() << "BYDAY=" << days.str();
            values.next();
        }

        if (this->until.valid()) {
            values.out() << "UNTIL=" << this->until;
            values.next();
        }

        if (this->bySecond.size()) {
            values.out() << "BYSECOND=" << this->bySecond;
            values.next();
        }

        if (this->byMinute.size()) {
            values.out() << "BYMINUTE=" << this->byMinute;
            values.next();
        }

        if (this->byHour.size()) {
            values.out() << "BYHOUR=" << this->byHour;
            values.next();
        }

        if (this->byMonthDay.size()) {
            values.out() << "BYMONTHDAY=" << this->byMonthDay;
            values.next();
        }

        if (this->byMonth.size()) {
            values.out() << "BYMONTH=" << this->byMonth;
            values.next();
        }

        if (this->byYearDay.size()) {
            values.out() << "BYYEARDAY=" << this->byYearDay;
            values.next();
        }

        if (this->byWeekNo.size()) {
            values.out() << "BYWEEKNO=" << this->byWeekNo;
            values.next();
        }

        if (this->bySetPos.size()) {
            values.out() << "BYSETPOS=" << this->bySetPos;
            values.next();
        }

        values.str(out);
    }

    std::ostream& operator << (std::ostream& out, const RRule::ptr& r) {
        r->str(out);
        return out;
    }

    std::ostream& operator << (std::ostream& out, const RRule& r) {
        r.str(out);
        return out;
    }

    std::ostream& operator << (std::ostream& out, const RRule::Day_pair& dp) {
        int idx;
        DateTime::Day day;
        unpack(dp, idx, day);
        if (idx)
            out << idx << day;
        else
            out << day;
        return out;
    }
}
