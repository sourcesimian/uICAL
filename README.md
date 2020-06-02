uICAL  <!-- omit in toc -->
====

***Lightweight C++ library for parsing ICAL calendar format***

This project implements the [iCalendar RFC-5545](https://icalendar.org/RFC-Specifications/iCalendar-RFC-5545/) standard in pure C++ to the C++ 11 standard. It is targeted at small embedded, Arduino and IoT devices with limited resources and no operating system. uICAL can however be used on any platform which is C++ 11 compliant.

At the time of writing I could not find a suitable ICAL parser implementation for my use case, thus I set out to write uICAL. Fortunately, I underestimated the difficuilty in correctly implementing the ICAL specification, or I would never have started. The code now satisfies all the test cases I've been able to find, and runs quite sucessfully in an [ESP8266](https://github.com/sourcesimian/uICAL/blob/master/examples/sonoff-basic/README.md) microcontroller.

Coincedentally during the writing of uICAL the [iCalendar.org](https://icalendar.org) made this [IoT](https://icalendar.org/iot.html) calendar reading service available which is a neat way of off-loading the task from an under resourced IoT device.

- [Availability](#availability)
- [API](#api)
- [C++ Library](#c-library)
- [Arduino C++](#arduino-c)
  - [License](#license)

# Availability

The library is available as pure C++, Python and Micropython.
Arduino PlatformIO library.

# API

# C++ Library
```
make test-cpp
```


# Arduino C++
Checkout uICAL into `./lib/`.

Then in source use:
```
#include <uICAL.h>
...
auto ical = uICAL::Calendar(...);
```





License
----

[MIT](LICENSE)
