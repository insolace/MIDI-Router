# MIDI-Router
Created by Eric Bateman (eric@timeline85.com)
Released under GPLv3

![](Images/Collage%20LG.jpg)

MIDIRouter is a hardware MIDI processor.  It routes and translates messages to/from hardware DIN, USB Devices, and a USB Host (DAW).  

INSTALLATION:
* Hardware - prototype dev kits available in limited quantities for active contributors/devs, email eric (at) timeline85.com for more info.  Otherwise see hardware listed below and DIY.
* Libraries - see “Library Info.txt” before attempting to compile!
* First time compiling - Select Teensy 3.6 and port etc, set USB TYPE to "Serial + MIDIx16".

Accessing the SD card:
https://github.com/insolace/Arduino_ZModem

Hardware:

Teensy 3.6 with USB panel mount pig-tails, red 5v power cut from USB-B (DAW) connector, leave 5v connected for USB-A (DEVICE) connector. 
https://www.pjrc.com/store/teensy36.html

Display - ER-TFTM050A2-3-3661
https://www.buydisplay.com/default/5-inch-tft-lcd-display-capacitive-touchscreen-ra8875-controller-800x480
-This display uses the RA8875 display (SPI) and GSL1680 (I2S) touch drivers.

MIDI hardware follows electrical spec:
https://www.midi.org/specifications-old/item/midi-din-electrical-specification
(Transmit resistors are 47ohm for 3.5v serial from TEENSY)

DMX not yet incorporated into prototype, but will share serial6 TX line with MIDI6, and use this circuit:
https://i.stack.imgur.com/0ZBnO.png

Eurorack connections use TEENSY I/O and DAC with op-amps as appropriate.
