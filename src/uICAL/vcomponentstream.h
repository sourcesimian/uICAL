/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_componentstream_h
#define uical_componentstream_h

#include "uICAL/vlinestream.h"

namespace uICAL {
    class VComponentStream {
        public:
            VComponentStream(VLineStream& stm);

            VComponent::ptr next();

        protected:
            void load(VComponent::ptr comp);
            VLineStream& stm;
    };
}
#endif
