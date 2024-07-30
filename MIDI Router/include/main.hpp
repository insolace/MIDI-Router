#ifndef MAIN_H
#define MAIN_H

#include "Arduino.h"
//#include <stdint.h>
#include "MIDIRouter_Library.h"

#define FONT5X7_H // this will clear a bogus compiler warning caused by platformio including headers from the ssd1351/glcdfont.c library that we don't use

// Set parameters
/// MIDI_SERIAL_SUPPORT = add hardware serial support for midi ports
#define MIDI_SERIAL_SUPPORT
/// MIDI_USB_SUPPORT = add usb support for midi ports
#define MIDI_USB_SUPPORT
/// SDCARD_SUPPORT = add SD card support
#define SDCARD_SUPPORT
/// TFT_DISPLAY = add TFT display support
#define TFT_DISPLAY
/// STARTUP_PICTURE = show the startup picture loaded from the sd card
#define STARTUP_PICTURE




extern usb_serial_class Serial;

USING_NAMESPACE_MIDIROUTER
extern MIDIRouter_Lib router;

#endif/* MAIN_H */
