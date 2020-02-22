/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/
#ifndef uical_base_h
#define uical_base_h

namespace uICAL {
    class string;
    class ostream;
    
    class Base {
        public:
            virtual ~Base() = default;
            virtual void str(ostream& stm) const = 0;
            string as_str() const;
    };
}

#endif