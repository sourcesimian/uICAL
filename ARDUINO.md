uICAL on Arduino  <!-- omit in toc -->
================

The library uses C++ exceptions, which are typically disabled by Arduino IDE's compiler configurations. You need to edit the compiler flags and add the `-fexceptions` flag. 

For example, for Arduino's `ESP8266` package, edit the `boards.txt` file found in `/packages/esp8266/hardware/esp8266/x.x.x/` where your Arduino installation places board files (on Mac it is in `~/Library/Arduino15/`). Edit the following line as follows: `generic.menu.exception.disabled.build.exception_flags=-fexceptions`
