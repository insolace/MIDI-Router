# MIDI-Router
Created by Eric Bateman (eric@timeline85.com)
Released under GPLv3

Libraries: see “Library Info (README).txt” before attempting to compile!


Hardware:

Teensy 3.6 with USB panel mount pig-tails, red 5v power cut from USB-B (DAW) connector, left for USB-A (DEVICE) connector. 
https://www.pjrc.com/store/teensy36.html

Display - ER-TFTM050A2-3-3661
https://www.buydisplay.com/default/5-inch-tft-lcd-display-capacitive-touchscreen-ra8875-controller-800x480

MIDI hardware follows electrical spec:
https://www.midi.org/specifications-old/item/midi-din-electrical-specification
(Transmit resistors are 47ohm for 3.5v serial from TEENSY)

DMX not yet incorporated into prototype, but will share serial6 TX line with MIDI6, and use this circuit:
https://i.stack.imgur.com/0ZBnO.png

Eurorack connections use TEENSY I/O and op-amps as appropriate.
