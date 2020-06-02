Online Timeswitch - Sonoff Basic <!-- omit in toc -->
================================

The Sonoff Basic WiFi controlled switch from [Itead](https://www.itead.cc/) is an exellent little device. It is inexpensive and easily reprogrammed for custom use. It is one of the many popular devices used in the  [Tasmota](https://tasmota.github.io/docs/) IoT and home automation ecosystem. It is based on the [ESP8266](https://en.wikipedia.org/wiki/ESP8266) microcontroller.

This project was developed to showcase the [uICAL](https://github.com/sourcesimian/uICAL) library and re-purposes the Sonoff Basic as a standalone online timeswitch, scheduled from an online calendar (in ICAL format). It is an ideal product for people who do not wish to commit to integrated IoT home automation, but would like one or two lights to go on and off according to a schedule in an online calendar.

- [Programming](#programming)
  - [The Device](#the-device)
  - [Build the Firmware](#build-the-firmware)
  - [Preparations to Flash the Firmware](#preparations-to-flash-the-firmware)
  - [Electrical Safety](#electrical-safety)
  - [Flashing the Firmware](#flashing-the-firmware)
- [Usage](#usage)
  - [Configuring the Timeswitch](#configuring-the-timeswitch)
    - [Calendar URL](#calendar-url)
    - [Host Fingerprint](#host-fingerprint)
    - [Poll Period](#poll-period)
  - [Force a Calendar Update](#force-a-calendar-update)
  - [Manual On](#manual-on)

# Programming
## The Device
This project was developed for the [Sonoff Basic R2](https://sonoff.tech/product/wifi-diy-smart-switches/basicr2). The PCB has the markings: `Sonoff RF R2 POWER V1.0`.

## Build the Firmware
Checkout the uICAL repository. Open this folder in [VSCode](https://code.visualstudio.com/) using the [PlatformIO](https://platformio.org/) plugin. In the PlatformIO sidebar click "**Build**" in the "Project Tasks" pane.

## Preparations to Flash the Firmware
To flash the firmware to the Sonoff Basic you will need:
* FTDI FT232R USB to TTL Serial Breakout Module
* 4-way header
* 4-way TTL jumper cable
* soldering iron

There are many guides and videos online on how to do this, this one is an [example](https://tasmota.github.io/docs/devices/Sonoff-Basic/). Search the internet to find out more.

## Electrical Safety
**Never mix mains power and TTL/USB!**

Only ever connect your Sonoff Basic to the mains **OR** the FT232R USB to TTL. Never both!

Disconnect the the mains before connecting the 4-way TTL jumper cable. Before connecting to mains power, disconnect the 4-way TTL jumper cable, and close the case!

## Flashing the Firmware
Plug the FTDI FT232R USB to TTL into your USB port and find the path to the serial device in `/dev/`. Set the value of `upload_port = /dev/cu....` in `platformio.ini` to the serial device. Disconnect the USB once you have found it.

Connect the 4-way TTL jumper cable to the Sonoff board - make sure that:
* the FTDI FT232R USB to TTL is in **3.3V mode**,
* the 4 TTL jumper cables: `3.3V`, `TX`, `RX`, `GND` are correctly connected on both ends.

While pressing the button on the Sonoff, connect the USB cable - this puts the board in flashing mode.

In the PlatformIO sidebar click "**Upload**" in the "Project Tasks" pane.

# Usage
Once configured, the Online Timeswitch will switch the relay on for the duration of any events in your calendar with the selected Event Title.

## Configuring the Timeswitch
Power on the device. If it is the first time it will automatically go into config mode. The LED will flash rapidly. At any other time, to enter config mode press and hold the button for 10 seconds or more.

Connect your laptop/phone/tablet to the `OnlineTimeswitch-?????????` WiFi hotspot. If the configuration page does not popup, browse to [http://sonoff-basic.config](http://online-timeswitch.config).

Enter:
* WiFi Name
* WiFi Password
* ICAL URL - link to your online ICAL format calendar
* Host Fingerprint - can be left blank. See below if you wish to set it.
* Poll Period - how often you want the timeswitch check for changes in your calendar. 3600 seconds is a good default.
* Manual On Duration (s) - The maximum amount of time which the manual ON override will be active. 3600 seconds is a good default.
* Calendar Event Title - the name you will use for the event in your calendar.

Click "**Update**". And then press the button to exit config mode. The LED should flash slowly while the device is connecting to your WiFi and updating the calendar, and if sucessful will switch off. If the LED continues to flicker there is some sort of connectivity or configuration problem.

### Calendar URL
The calendar URL you use will depend on which calendar system you are using. The Online Timeswitch can only understand the [iCAL format](https://icalendar.org/RFC-Specifications/iCalendar-RFC-5545/).

If you are using [Google Calendars](https://calendar.google.com/): in the **My calendars** sidebar hover over your chosen calendar, and click the **Options for ...** icon, and then **Settings and Sharing**. Scroll down to **Secret address in iCal format**, and use that URL. You can use some of the other URLs, but that I'll leave up to you to figure out.

### Host Fingerprint
To ensure that your Online Timeswitch is fetching your calendar from the authentic host you can add a host fingerprint. However, if you set it wrong the Online Timeswitch will fail to fetch your  calendar.

It is important to note that the fingerprint may not be the usual one due to some optimisations in the HTTPS library used, for more detail see [BearSSL Fingerprint Lookup](https://github.com/sourcesimian/uICAL/blob/master/examples/BearSslFingerprint/README.md).

The fingerprint in this case for https://calendar.google.com/ is `f3:c2:ff:9d:10:7e:b8:fc:96:13:da:de:e8:43:1d:2f:65:ae:2c:fa`.

### Poll Period
Typically a good value to use is one hour (3600 seconds). Be careful not to set the period too short, because then you may overload the calendar server, and possibly even get your IP address blocked. A setting of an hour means that at worst case it will take one hour before the Online Timeswitch becomes aware of any changes you have made to the calendar.

## Force a Calendar Update
If you have made a change to your calendar and you want the Online Timeswitch to become aware of it immediately: then press and hold the button for ~5 seconds. The LED will light, while the update is taking place, and will go off again if the update was a success.

## Manual On
As with electro-mechanical time switches it is sometimes convienient to have a manual on mode. Short press the button to engage the manual on mode. The LED will stay lit to indicate that manual on mode is engaged. Short press the button again to exit manual on mode. The Online Timeswitch will then continue to follow your calendar. If you leave the manual on mode engaged the Online Timeswitch will automaticaly exit after the manual on duration you have configured.

