/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_icalline_h
#define uical_icalline_h

#include "uICAL/base.h"

namespace uICAL {
    class VLine : public Base {
        public:
            using ptr = std::shared_ptr<VLine>;

            static VLine::ptr init();
            static VLine::ptr init(const string& line);
            VLine();
            VLine(const string& line);

            bool empty() const;
            string name;
            std::map<string, string> params;
            string value;
            string getParam(const string& key);

            void str(ostream& out) const;

        private:
            void readParams(const string& str);
    };

    class VLineStream {
        public:
            using ptr = std::shared_ptr<VLineStream>;

            VLineStream(istream& ical);
            // const VLine::ptr peek();
            // void pop();
            const VLine::ptr next();
        protected:
            istream& ical;
            // string current;
    };
}
#endif
