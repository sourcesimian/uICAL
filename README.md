# uICAL
Lightweight C++ and Python library for reading ICAL calendar format

## Implementation
Implemented in C++ to the C++11 standard. based on [iCalendar RFC-5545](https://icalendar.org/RFC-Specifications/iCalendar-RFC-5545/).

# API

# C++ Library
```
make test-cpp
```

# Python
```
make test-python
```

In your virtualenv:
```
python3 setup.py build install
```

# Arduino C++
Checkout uICAL into `./lib/`.

Then in source use:
```
#include <uICAL.h>
...
auto ical = uICAL::Calendar(...);
```

# MicroPython - ESP32
ref: [MicroPython C modules](https://docs.micropython.org/en/latest/develop/cmodules.html)
```
git checkout https://github.com/micropython/micropython
```

Setup as described in [`micropython/ports/esp32/README.md`](https://github.com/micropython/micropython/blob/master/ports/esp32/README.md)

Verify setup with:
```
cd micropython/ports/esp32
make all
```

Checkout uICAL to a `modules` directory e.g.:
```
├── usermod/
│   ├──uICAL/
│  ...  
└── micropython/
    ├──ports/
   ... ├──esp32/
      ...
```

Then build with:
```
cd micropython/ports/esp32
make USER_C_MODULES=../../../modules all
make USER_C_MODULES=../../../usermod CFLAGS_EXTRA=-DMODULE_UICAL_ENABLED=1 all
```

> NOTE: To flash the firmware you must have your ESP32 module in the bootloader mode and connected to a serial port on your PC

The first time you install MicroPython:
```
make erase
```

Flash ther MicroPython firmware:
```
make deploy
```




License
----

[MIT](LICENSE)


**Free Software, Hell Yeah!**

[//]:
    [dill]: <https://github.com/joemccann/dillinger>
    
 