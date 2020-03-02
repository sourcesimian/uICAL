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

    ostream& operator << (ostream& out, const Base& b);

    template<typename T>
    ostream& operator << (ostream& out, const std::shared_ptr<T>& b) {
        b->str(out);
        return out;
    }
}
#endif
