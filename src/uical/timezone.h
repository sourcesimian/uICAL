#ifndef uical_timezone_h
#define uical_timezone_h

namespace uICAL {
    class VComponent;

    class Timezones {
        public:
            using ptr = std::shared_ptr<Timezones>;

            Timezones(VComponent& calendar);

            std::string getOffset(const std::string tzId);

        private:
            std::map<std::string, std::string> timezone_map;
    };
}
#endif
