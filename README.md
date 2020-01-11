# qnapctl

A tiny system daemon to control and react to QNAP hardware, including:

* the panel LCD screen (toggle backlight and change displayed text)
* the panel LCD buttons, usually for navigating the LCD display
* the *STATUS* and *USB* LEDs
* the *USB COPY* button
* the disk *ERROR* LEDs

Also includes a demo program that makes use of the API exposed by the daemon.

## Overview

Beware: this was developped and tested exclusively on the QNAP device I own, model TS-453 Pro. This code most likely won't work on different hardware.

TS-453 Pro features:

* an A125 LCD panel with two 2×16 chars, backlight and two control buttons
* a 4 disk bay, with activity and error LEDs for each of the 4 bays
* some extra LEDs: dual-color status (green & red), USB activity and non-overridable network activity

### qnapd

`qnapd` is a C++ (Qt) daemon that:

* reads from internal serial device `/dev/ttyS1` to intercept LCD button presses (*ENTER* and *SELECT*)
* writes to internal serial device `/dev/ttyS1` to toggle LCD backlight and change LCD text
* polls a raw I/O port to intercept *USB COPY* button presses
* writes to a few raw I/O ports to toggle the various front panel LEDs

The daemon receives orders and broadcasts events using the system DBus. The DBus API is self-documented in the [XML interface](dbus_interface.xml.in).

### qnapctl

`qnapctl` is a C++ (Qt) program that communicates with `qnapd` to provide a few basic features:

* blinks the green status LED faster or slower depending on load average
* blinks the red status LED when there are failed systemd services
* displays a few basic info on the LCD display eg. NIC IP addresses, using LCD buttons to navigate the various info

## License

MIT, see `LICENSE`.
