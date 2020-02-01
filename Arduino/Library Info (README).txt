MIDI Router uses the following modified libraries from github.com/insolace.  Put them in your sketchbook/libraries folder and they should take precedence over the libraries included with the TeensyDuino install:

GSL1680.h
Adafruit_GFX.h
Adafruit_RA8875.h

Also add:
SDfat - https://github.com/greiman/SdFat

Arduino standard libraries:
Wire.h
stdint.h
SPI.h

The Arduino standard MIDI library is out of date.  You need to replace it with the current version that fixes clocking issues:
https://github.com/FortySevenEffects/arduino_midi_library

Teensy 3.6 libraries from Paul Stoffregen (included with TeensyDuino install):
Bounce2.h
USBHost_t36.h
