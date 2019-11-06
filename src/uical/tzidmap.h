#ifndef uical_tzinfo_h
#define uical_tzinfo_h

namespace uICAL {
    class VComponent;

    class TZIdMap {
        public:
            using ptr = std::shared_ptr<TZIdMap>;

            static ptr init(VComponent& calendar);
            TZIdMap(VComponent& calendar);

            int getOffset(const std::string tzId);

            void str(std::ostream& out) const;

        protected:
            int parseOffset(const std::string offset) const;

            typedef struct {
                int offset;
                std::string name;
            } attribs_t;
            
            std::map<std::string, attribs_t> id_attrib_map;
    };
}
#endif
