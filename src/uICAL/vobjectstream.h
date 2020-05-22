/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vobjectstream_h
#define uical_vobjectstream_h

namespace uICAL {
    class VObject;
    class VLineStream;

    class VObjectStream {
        public:
            using lineP_t = std::function<bool (const string parent, const string line)>;

            VObjectStream(VLineStream& stm, lineP_t useLine);

            VObject_ptr nextObject(bool recurse);

        protected:
            string nextObjectName();
            void loadObject(string objName, VObject_ptr& obj, bool recurse);

            VLineStream& stm;
            lineP_t useLine;
    };
}
#endif
