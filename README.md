uICAL  <!-- omit in toc -->
====

***Library for parsing iCalendar file format***

This project implements the [iCalendar RFC-5545](https://icalendar.org/RFC-Specifications/iCalendar-RFC-5545/) standard in pure C++ to the C++ 11 standard and minimal STL. It is targeted at small embedded, Arduino and IoT devices with limited resources and no operating system. uICAL can however be used in any project which is C++ 11 compliant.

At the time of writing I could not find a suitable iCalendar parser implementation for my use case, thus I set out to write uICAL. Fortunately, I underestimated the difficuilty in correctly implementing the ICAL specification, particularly the complexity of the [Recurrence (RRULE) property](https://icalendar.org/iCalendar-RFC-5545/3-8-5-3-recurrence-rule.html), or I would never have started. The code now satisfies all the RRULE test cases I've found so far, and runs quite sucessfully in an [ESP8266](https://github.com/sourcesimian/uICAL/blob/master/examples/sonoff-basic/README.md) microcontroller.

Coincedentally during the writing of uICAL the [iCalendar.org](https://icalendar.org) made this [IoT](https://icalendar.org/iot.html) calendar reading service available which is a neat way of off-loading the task from an under resourced IoT device.

If you are looking for a main stream implementation of iCalendar then you are probably looking for [Libical](https://libical.github.io/libical/).

- [Examples](#examples)
  - [Get All Events Between Two Times (C++ STL)](#get-all-events-between-two-times-c-stl)
  - [Event Filtering (Arduino)](#event-filtering-arduino)
- [uICAL Users](#uical-users)
- [Optimisations](#optimisations)
- [Building uICAL](#building-uical)
  - [C++ STL](#c-stl)
  - [Arduino](#arduino)
  - [Python Module](#python-module)
  - [Micropython Module](#micropython-module)
- [Development](#development)
- [License](#license)

# Examples

## Get All Events Between Two Times (C++ STL)
Here we load the calendar and then iterate all the events between the two times given.
```
#include <uICAL.h>

void main () {
    std::ifstream fstm(std::string("test/data/ical1.txt"));
    
    uICAL::istream_stl istm(fstm);
    auto cal = uICAL::Calendar::load(istm);
    
    uICAL::DateTime begin("20190901T102000Z");
    uICAL::DateTime end("20190931T103000Z");
    
    auto calIt = uICAL::new_ptr<uICAL::CalendarIter>(cal, begin, end);
    
    while(calIt->next()) {
        std::cout << calIt->current() << std::endl;
    }
}
```

## Event Filtering (Arduino)

```
bool MyClass::loadStream(Stream& stm) {
    try {
        uICAL::istream_Stream istm(stm);
        this->cal = uICAL::Calendar::load(istm, [=](const uICAL::VEvent& event){
            return this->useEvent(event);
        });
        return true;
    }
    catch (uICAL::Error ex) {
        log_error("%s: %s", ex.message.c_str(), "! Failed loading calendar");
        return false;
    }
}

bool MyClass::useEvent(const uICAL::VEvent& event) {
    // ...
}

void MyClass::printEvents(uICAL::DateTime begin, uICAL::DateTime end) {
    uICAL::CalendarIter_ptr calIt = uICAL::new_ptr<uICAL::CalendarIter>(this->cal, begin, end);
    while (calIt->next()) {
        uICAL::CalendarEntry_ptr entry = calIt->current();
        Serial.println(entry.as_str().c_str());
    }
```

# uICAL Users
* [Sonoff Basic as an Online Timeswitch](examples/sonoff-basic/README.md)

# Optimisations
To keep the uICAL libary as portable as possible I used as little C++ 11 as I could and made no dependency on operating system primitives. And then further to this point, practicality came along and pushed me to use even less. My initial implementation, which made use of the `<string>` and `<?stream*>` libraries was simply too big to fit into my chosen device. Thus I added the `<uICAL::string>` and `<uICAL::stream_*>` adaptors. These allow for alternative implementations to be swapped in. In the Arduino environment that means the `String` and `Stream`, however in a normal C++ environment that can still be the usual libraries.

Additionally in the IoT and Arduino space we have very limited heap space, _yes yes and we probably shouldn't even be using a heap_, so we need to be super conservative about how we use it. This means adopting a streaming philosophy and also freeing as much as we can as soon as we can. To this effort the implementaiton of the `VObjectStream` allows for immediate freeing of lines and components that the code will not consume.

Further at the user level, see the [Event Filtering (Arduino)](#event-filtering-arduino) example above, `uICAL::Calendar::load` can take an `uICAL::VEvent` filtering predicate, so that once we have loaded the full ICAL stream our calendar only aggregates the event objects we will use.

# Building uICAL
## C++ STL
The C++ source is available in [src/](src/), no code pre-configuration is required. Simply include uICAL.h, compile and link.

## Arduino
See [ARDUINO.md](ARDUINO.md)

## Python Module
See [python-module/README.md](python-module/README.md)

## Micropython Module
> uICAL support in Micropython is still in progress.

See [micropython-module/README.md](micropython-module/README.md)

# Development
See [DEVELOPMENT.md](DEVELOPMENT.md)

# License

In the spirit of the Hackers of the [Tech Model Railroad Club](https://en.wikipedia.org/wiki/Tech_Model_Railroad_Club) from the [Massachusetts Institute of Technology](https://en.wikipedia.org/wiki/Massachusetts_Institute_of_Technology), who gave us all so very much to play with. The license is [MIT](LICENSE).
