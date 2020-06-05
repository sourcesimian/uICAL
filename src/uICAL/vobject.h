/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vobject_h
#define uical_vobject_h

#include "uICAL/base.h"

namespace uICAL {
    class VLine;
    class VObjectStream;

    class VObject : public Base {
        public:
            VObject();

            virtual const string& getName() const;

            VLine_ptr getPropertyByName(const string& name) const;
            using vector = std::vector<VObject_ptr>;
            vector listObjects(const string& name) const;

            void str(ostream& out) const;

        protected:

            string name;
            std::vector<VLine_ptr> lines;
            std::vector<VObject_ptr> children;

            friend class VObjectStream;
    };
}
#endif
