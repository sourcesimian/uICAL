#ifndef uical_tz_h
#define uical_tz_h


namespace uICAL {
    using seconds_t = long long int;
    
    class DateStamp;

    class TZ {
        public:
            using ptr = std::shared_ptr<TZ>;

            static ptr undef();
            static ptr unaware();
            static ptr init(int offsetMins);
            static ptr init(const std::string tz);
            TZ();
            TZ(bool aware);
            TZ(int offsetMins);
            TZ(const std::string tz);

            virtual ~TZ() {}

            virtual seconds_t toUTC(seconds_t timestamp) const;
            virtual seconds_t fromUTC(seconds_t timestamp) const;

            virtual void str(std::ostream& out) const;

            static int parseOffset(const std::string offset);
            static void offsetAsString(std::ostream& out, int offsetMins);
        
        private:
            int offsetMins;
            bool aware;
    };
    
    std::ostream & operator << (std::ostream &out, const TZ::ptr& tz);
}
#endif
