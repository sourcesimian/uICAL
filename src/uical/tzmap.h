/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_tzinfo_h
#define uical_tzinfo_h

namespace uICAL {
    class VComponent;

    class TZMap {
        public:
            using ptr = std::shared_ptr<TZMap>;

            static ptr init();
            static ptr init(VComponent& calendar);
            TZMap();
            TZMap(VComponent& calendar);

            std::string findId(const std::string nameOrId) const;

            int getOffset(const std::string tzId);
            std::string getName(const std::string tzId);

            void str(std::ostream& out) const;

        protected:
            void add(const std::string id, const std::string name, const std::string tz);
            int parseOffset(const std::string offset) const;

            typedef struct {
                int offset;
                std::string name;
            } attribs_t;
            
            std::map<std::string, attribs_t> id_attrib_map;
    };
}
#endif
