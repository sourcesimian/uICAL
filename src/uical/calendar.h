#ifndef uical_calendar_h
#define uical_calendar_h


namespace uICAL {
    class Calendar {
        public:
            using ptr = std::shared_ptr<Calendar>;
            static ptr init(const std::string ical);
            Calendar(const std::string ical);

            void str(std::ostream& out) const;

        protected:
    };

    std::ostream & operator << (std::ostream &out, const Calendar::ptr &c);
    std::ostream & operator << (std::ostream &out, const Calendar &c);
}
#endif
