#ifndef uical_tz_h
#define uical_tz_h

namespace uICAL {
    class DateStamp;

    class TZ {
        public:
            using ptr = std::shared_ptr<TZ>;

            static ptr none();
            static ptr init(const std::string tz);
            
            TZ();
            TZ(const std::string tz);

            void str(std::ostream& out) const;

            TZ& operator= (const TZ &tz);

        protected:
            int offset;
    };
}
#endif
