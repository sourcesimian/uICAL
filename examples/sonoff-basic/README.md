# Online Timeswitch - Sonoff Basic

The Sonoff Basic WiFi controlled switch from [Itead](https://www.itead.cc/) is an exellent little device. It is inexpensive and easily reprogrammed for custom use. It is one of the many popular devices used in the  [Tasmota](https://tasmota.github.io/docs/) IoT and home automation ecosystem.

This project was developed to showcase the [uICAL](https://github.com/sourcesimian/uICAL) library and re-purposes the Sonoff Basic as a standalone online timeswitch, scheduled from an online calendar (in ICAL format). It is an ideal product for people who do not wish to commit to integrated IoT home automation, but would like one or two lights to go on and off according to a schedule in an online calendar.

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

There are many guides an dvideos online on how to do this, this one is an [example](https://tasmota.github.io/docs/devices/Sonoff-Basic/). Search the internet to find out more.

# Electrical Safety
**Never mix mains power and TTL/USB!**
Only ever connect your Sonoff Basic to the mains **OR** the FT232R USB to TTL. Never both! Disconnect the the mains before connecting the 4-way TTL jumper cable. Disconnect the 4-way TTL jumper cable, and close the case of the Sonoff Basic, before connecting to mains power.

# Flashing the Firmware
Plug the FTDI FT232R USB to TTL into your serial port and find the path to your serial device in `/dev/`. Set the value of `upload_port = /dev/cu....` in `platformio.ini` to your serial device. Disconnect the USB once you have found it.

Connect the 4-way TTL jumper cable to the Sonoff board - make sure that:
* the FTDI FT232R USB to TTL is in **3.3V mode**,
* the 4 TTL jumper cables: 3.3V, TX, RX, GND are connected correctly on both ends.

While pressing the button on the Sonoff, connect the USB cable - this puts the board in flashing mode.

In the PlatformIO sidebar click "**Upload**" in the "Project Tasks" pane.

# Configuring the Timeswitch
Device configuration has been made relatively easy with the [SimpleConfigWiFiAP](https://github.com/sourcesimian/uICAL/examples/sonoff-basic/lib/SimpleConfigWiFiAP/) library. Power on the device either using either the FTDI FT232R USB to TTL or connecting the mains (takeing the relevant saftey precautions). Press and hold the button for 10s or more. The LED will start to flash rapidly. Connect your laptop/or phone to the `OnlineTimeswitch-?????????` WiFi hotspot. If the configuration page does not popup, open [http://sonoff-basic.config](http://online-timeswitch.config).

Enter:
* WiFi Name
* WiFi Password
* ICAL URL - link to your online ICAL format calendar
* Host Fingerprint - can be left blank. See below if you wish to set it.
* Poll Period - how often you want the timeswitch to update from your calendar. Every 5 minutes is 300s. Every hour is 3600s. Every day 86400s.
* Relay Event Summary - the name you will use for that event in the calendar.
* Manual On Maximum Duration (s) - The maximum amount of time which the manual on over ride will be active.

Click "**Update**". And then press the button on the Sonoff to exit the config mode. The LED should flicker while the divice is connecting to your WiFi and updating the calendar, and then should switch off. If the LED continues to flicker there is some sort of connectivity or configuration problem.
