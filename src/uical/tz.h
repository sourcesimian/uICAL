#ifndef uical_tz_h
#define uical_tz_h


namespace uICAL {
    using timestamp_t = unsigned long long int;
    
    class DateStamp;

    class TZ {
        public:
            using ptr = std::shared_ptr<TZ>;

            static ptr none();
            static ptr init(int offsetMins);
            static ptr init(const std::string tz);
            
            TZ();
            TZ(int offsetMins);
            TZ(const std::string tz);

            virtual ~TZ() {}

            timestamp_t toUTC(timestamp_t timestamp) const;
            timestamp_t fromUTC(timestamp_t timestamp) const;

            //TZ& operator= (const TZ &tz);

            void str(std::ostream& out) const;

            static int parseOffset(const std::string offset);
            static void offsetAsString(std::ostream& out, int offsetMins);
        
        protected:
            int offsetMins;
    };
}
#endif
