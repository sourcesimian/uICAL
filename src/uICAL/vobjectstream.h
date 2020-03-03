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
            VObjectStream(VLineStream& stm, VObject_ptr& obj);

            const string& nextChild();
            VObject_ptr loadChild();
            void skipChild();

        protected:
            VLine_ptr nextObj(VObject_ptr& obj, bool skip);
            void loadObj(VObject_ptr& obj, VObject_ptr& child, bool skip);

            VLineStream& stm;
            VObject_ptr& obj;
            VObject_ptr child;

    };
}
#endif
