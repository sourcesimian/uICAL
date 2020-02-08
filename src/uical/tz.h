#ifndef uical_tz_h
#define uical_tz_h


namespace uICAL {
    class DateStamp;

    class TZ {
        public:
            using ptr = std::shared_ptr<TZ>;

            static ptr init(int offsetMins);
            static ptr init(const std::string tz);

            static ptr undef();
            static ptr unaware();

            TZ();
            TZ(bool aware);
            TZ(int offsetMins);
            TZ(const std::string tz);

            virtual ~TZ() {}

            virtual seconds_t toUTC(seconds_t timestamp) const;
            virtual seconds_t fromUTC(seconds_t timestamp) const;

            bool is_aware() const;

            virtual void str(std::ostream& out) const;
            std::string str() const;

            static int parseOffset(const std::string offset);
            static void offsetAsString(std::ostream& out, int offsetMins);
        
        protected:
            bool aware;

        private:
            int offsetMins;
    };
    
    std::ostream & operator << (std::ostream &out, const TZ::ptr& tz);
}
#endif
