/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

class ButtonMonitor {
    public:
        ButtonMonitor(uint8_t pin);

        long getPressed();

    protected:
        bool isPressed();

        const uint8_t pin;
        unsigned long pressMillis;
};