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

            VObject_ptr nextChild();
            void loadChild();

        protected:
            VObject_ptr nextObj(VObject_ptr& obj);
            void loadObj(VObject_ptr& obj, VObject_ptr& child);

            VLineStream& stm;
            VObject_ptr& obj;
            VObject_ptr child;

    };
}
#endif
