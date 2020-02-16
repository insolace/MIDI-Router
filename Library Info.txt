MIDI Router Library Configuration

Remove/archive the following libraries from the Arduino/Teensy default installation:

hardware/teensy/avr/libraries/Adafruit_GFX
hardware/teensy/avr/libraries/Adafruit_RA8875
hardware/teensy/avr/libraries/RA8875
hardware/teensy/avr/libraries/MIDI

Clone these libraries to your sketchbook/libraries folder:

https://github.com/insolace/GSL1680
https://github.com/insolace/Adafruit-GFX-Library
https://github.com/insolace/Adafruit_RA8875
https://github.com/FortySevenEffects/arduino_midi_library
https://github.com/greiman/SdFat

Arduino standard libraries (included with the Arduino IDE install):
Wire.h
stdint.h
SPI.h

Teensy 3.6 libraries from Paul Stoffregen (included with TeensyDuino install):
Bounce2.h
USBHost_t36.h
