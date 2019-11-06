#ifndef uical_calendar_h
#define uical_calendar_h


namespace uICAL {
    class Calendar {
        public:
            using ptr = std::shared_ptr<Calendar>;
            static ptr init(std::istream& ical);
            Calendar(std::istream& ical);

            
            bool next();
            
            
            void str(std::ostream& out) const;

        protected:
            

    };

    std::ostream & operator << (std::ostream &out, const Calendar::ptr &c);
    std::ostream & operator << (std::ostream &out, const Calendar &c);
}
#endif
