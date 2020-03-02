/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_vlinestream_h
#define uical_vlinestream_h

namespace uICAL {
    class VLineStream {
        public:
            VLineStream(istream& ical);
            const VLine_ptr next();

        protected:
            istream& ical;
    };
}
#endif
