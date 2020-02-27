/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_icallinestream_h
#define uical_icallinestream_h

namespace uICAL {
    class VLineStream {
        public:
            using ptr = std::shared_ptr<VLineStream>;

            VLineStream(istream& ical);
            const VLine::ptr next();

        protected:
            istream& ical;
    };
}
#endif
