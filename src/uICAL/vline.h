/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vline_h
#define uical_vline_h

#include "uICAL/base.h"

namespace uICAL {
    class VLine : public Base {
        public:
            VLine(const string& line);

            string name;
            string value;
            string getParam(const string& key);

            void str(ostream& out) const;

        private:
            void readParams(const string& str);

            std::map<string, string> params;
    };
}
#endif
