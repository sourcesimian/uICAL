/*############################################################################
# Copyright (c) 2020 Source Simian  :  https://github.com/sourcesimian/uICAL #
############################################################################*/

class LedFlash {
    public:
        LedFlash(uint8_t pin);

        void state(bool on);
        void toggle();
        void flash(int on, int off = 0);

        void handle();

    protected:        
        void setState(bool on);
        bool getState();

        const uint8_t pin;
        unsigned flashOn;
        unsigned flashOff;
        unsigned long stateChangeMillis;
};