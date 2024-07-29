#include "midi.hpp"

#include <stdint.h>
#include "Arduino.h"
#include "main.hpp"
#include "MRPorts.h"
#include "cvDAC.hpp"
#include "touchscreen.hpp"
#include "storage.hpp"

// Hardware and USB Device MIDI
// https://github.com/FortySevenEffects/arduino_midi_library
// Create the Serial MIDI ports
// MIDI_CREATE_INSTANCE(Type, SerialPort, Name)
#ifdef MIDI_SERIAL_SUPPORT
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1); ///< Create MIDI 1 interace instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2); ///< Create MIDI 2 interace instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3); ///< Create MIDI 3 interace instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4); ///< Create MIDI 4 interace instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5); ///< Create MIDI 5 interace instance
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6); ///< Create MIDI 6 interace instance

midi::MidiInterface<midi::SerialMIDI<HardwareSerial>> *dinlist[6] =
{
        &MIDI1, &MIDI2, &MIDI3, &MIDI4, &MIDI5, &MIDI6
};

#endif // MIDI_SERIAL_SUPPORT


// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;              ///< @return USBHost USB Host
USBHub hub1(myusb);         ///< @return USBHub USB Hub 1
USBHub hub2(myusb);         ///< @return USBHub USB Hub 2
USBHub hub3(myusb);         ///< @return USBHub USB Hub 3
USBHub hub4(myusb);         ///< @return USBHub USB Hub 4

#ifdef MIDI_USB_SUPPORT
MIDIDevice midi01(myusb);   ///< @return MIDIDevice USB MIDI 1
MIDIDevice midi02(myusb);   ///< @return MIDIDevice USB MIDI 2
MIDIDevice midi03(myusb);   ///< @return MIDIDevice USB MIDI 3
MIDIDevice midi04(myusb);   ///< @return MIDIDevice USB MIDI 4
MIDIDevice midi05(myusb);   ///< @return MIDIDevice USB MIDI 5
MIDIDevice midi06(myusb);   ///< @return MIDIDevice USB MIDI 6
MIDIDevice midi07(myusb);   ///< @return MIDIDevice USB MIDI 7
MIDIDevice midi08(myusb);   ///< @return MIDIDevice USB MIDI 8
MIDIDevice midi09(myusb);   ///< @return MIDIDevice USB MIDI 9
MIDIDevice midi10(myusb);   ///< @return MIDIDevice USB MIDI 10

/// a  list of usb midi devices for easy array access
MIDIDevice * midilist[10] =
{
    &midi01, &midi02, &midi03, &midi04, &midi05, &midi06, &midi07, &midi08, &midi09, &midi10
};
#endif // MIDI_USB_SUPPORT


// clock stuff
boolean clockPulse[6] = {0, 0, 0, 0, 0, 0}; ///< clock pulse boolean for 6 digital outputs
int startCount = 0; ///< clock pulse start count

// =====================================================
// USB Devices
bool usbActive[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
bool usbWasActive[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

// =====================================================


/// Initial routing
/// matrix for routing
/// Each byte in the routing matrix is decoded thusly:
/// -------------------------------------------------
/// bit 0 = keyboard (note on/off, pitchbend, aftertouch, etc)
/// bit 1 = parameters (CC, NRPN/RPN, Sysex parameters etc)
/// bit 2 = transport (clock, start/stop)
/// bits 3-7 = channel filter (0 = all, 1-16 = 0-15 binary)
uint8_t routing[50][50] =    ///< [input port][output port]
{

    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Sysex
uint8_t sysexIDReq[] = {240, 126, 127, 6, 1, 247}; ///< SysEx ID Request

void initMidi()
{
	// ============================================================
    // MIDI setup
    // ============================================================

	// Turn on USB port, and profile DIN instruments with SysEx
    // Do this before loading WELCOME.BMP, allowing time for
    // USB enumeration and for MIDI devices to boot before being polled
    myusb.begin();

#ifdef MIDI_SERIAL_SUPPORT
    MIDI1.begin(MIDI_CHANNEL_OMNI);
    MIDI2.begin(MIDI_CHANNEL_OMNI);
    MIDI3.begin(MIDI_CHANNEL_OMNI);
    MIDI4.begin(MIDI_CHANNEL_OMNI);
    MIDI5.begin(MIDI_CHANNEL_OMNI);
    MIDI6.begin(MIDI_CHANNEL_OMNI);

    MIDI1.turnThruOff();
    MIDI2.turnThruOff();
    MIDI3.turnThruOff();
    MIDI4.turnThruOff();
    MIDI5.turnThruOff();
    MIDI6.turnThruOff();
#endif //MIDI_SERIAL_SUPPORT
}

void routeMidi()
{
    // ============================================================
    // RX Hardware DIN MIDI ports
    // ============================================================
    
    // Read messages arriving from the 6 hardware DIN ports

    for (int port = 0; port < 6; port++)
    {
        if (dinlist[port]->read())
        {
            byte type =          dinlist[port]->getType();
            byte data1 =         dinlist[port]->getData1();
            byte data2 =         dinlist[port]->getData2();
            byte channel =       dinlist[port]->getChannel();
            const uint8_t *sys = dinlist[port]->getSysExArray();
            if (type != 240)
            {
                transmitMIDI(type, data1, data2, channel, port);
            }
            else
            {
                transmitSysEx(data1 + data2 * 256, sys, port);
            }
        }
    }
    // ============================================================
    // RX USB DEVICE MIDI ports
    // ============================================================
    
    // Next read messages arriving from the (up to) 10 USB devices plugged into the MIDIROUTER USB devices port
    for (int port = 0; port < 10; port++)
    {
        if (midilist[port]->read())
        {
            uint8_t type =       midilist[port]->getType();
            uint8_t data1 =      midilist[port]->getData1();
            uint8_t data2 =      midilist[port]->getData2();
            uint8_t channel =    midilist[port]->getChannel();
            const uint8_t *sys = midilist[port]->getSysExArray();
            if (type != 240)
            {
                transmitMIDI(type, data1, data2, channel, port + 6);
            }
            else
            {
                transmitSysEx(data1 + data2 * 256, sys, port + 6);
            }
        }
    }
    
    // ============================================================
    // RX USB HOST MIDI ports
    // ============================================================
    
    // Read USB HOST (computer/DAW) messages the PC sends to Teensy, 16 ports. Reroute them based on matrix
    
    if (usbMIDI.read())
    {
        // get the USB MIDI message from computer, defined by these 5 numbers (except SysEX)
        byte type = usbMIDI.getType();
        byte data1 = usbMIDI.getData1();
        byte data2 = usbMIDI.getData2();
        byte channel = usbMIDI.getChannel();
        byte cable = usbMIDI.getCable() + 18; // 18 is offset for USB Host
        if (type != 240)
        {
            transmitMIDI(type, data1, data2, channel, cable);
        }
        else
        {
            transmitSysEx(data1 + data2 * 256, usbMIDI.getSysExArray(), cable);
            //Serial.println("DAW Sysex");
        }
    }
}

void transmitMIDI(int t, int d1, int d2, int ch, byte inPort)
{
    volatile int cR = 0;
    Serial.print("rxMIDI: t"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.print(ch); Serial.print(" inp:"), Serial.println(inPort);
    
    // Normal messages, first we must convert usbMIDI's type (an ordinary
    // byte) to the MIDI library's special MidiType.
    midi::MidiType mtype = (midi::MidiType)t;
    
    // Route to 6 MIDI DIN Ports
    for (int outp = 0; outp < 6; outp++)
    {
        cR = routing[inPort][outp];
        if (cR != 0)
        {
            if (filtRoute(mtype, ch, cR))
            {
                dinlist[outp]->send(mtype, d1, d2, ch);
            }
            Serial.print("tx d"); Serial.print(outp - 5); Serial.print(":");Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
        }
    }
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++)
    {
        cR = routing[inPort][outp];
        if (filtRoute(t, ch, cR))
        {
            Serial.print("tx u"); Serial.print(outp - 5); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            midilist[outp - 6]->send(t, d1, d2, ch);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++)
    {
        cR = routing[inPort][outp];
        if (filtRoute(t, ch, cR))
        {
            Serial.print("tx h"); Serial.print(outp - 17); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            usbMIDI.send(t, d1, d2, ch, outp - 18);
        }
    }
    
    // Route to 6 CV jacks
    for (int outp = 36; outp < 42; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            int curDac = outp - 36;
            cR = routing[inPort][outp];
            
            // test channel against filter, if not 0 or no match then exit
            int filtChan = cR >> 3;
            int filtType = cR & B00000111; // gather current filter setting
            
            if (filtChan == ch || filtChan == 0) // match channel
            {
                if (filtType == cvF_NOT || filtType == cvF_VEL) {                         // note on/off and/or velocity
                    if (t == 0x90) // note on (vel 0 = note off)
                    {
                        if (d2 == 0) // note off
                        {
                            setDIG(curDac, LOW); // GATE off
                        } else { // note on
                            if (filtType == cvF_NOT)
                            {
                                setDAC(curDac, CVnoteCal(d1, curDac)); // pitch of the note
                            } else {
                                setDAC(curDac, CVparamCal(d2, curDac)); // velocity
                            }
                            setDIG(curDac, HIGH); // GATE on
                            
                        }
                    } else if (t == 0x80) {
                        setDIG(curDac, LOW); // GATE off
                    }
                } else if (t == 0xE0 && filtType == cvF_PW) {                   // Pitch Wheel
                    int16_t pw14 = (d2 << 7 | d1);
                    setDAC(curDac, CV14bitCal( pw14, curDac)); // 14 bit converted to 16 bit value
                } else if (t == 0xB0 && d1 == 1 && filtType == cvF_MOD) {       // Mod Wheel
                    setDAC(curDac, CVparamCal(d2, curDac)); // mod value
                } else if (t == 0xB0 && filtType == cvF_CCs) {                  // CCs (any)
                    setDAC(curDac, CVparamCal(d2, curDac)); // CC value
                } else if (t == 0xB0 && d1 == 74 && filtType == cvF_CC74) {     // CC74 (MPE Y axis)
                    setDAC(curDac, CVparamCal(d2, curDac)); // MPE Y axis value
                } else if (t == 0xA0 && filtType == cvF_AT) {                   // AT (poly, any key)
                    setDAC(curDac, CVparamCal(d2, curDac)); // polyAT value, any key
                } else if (t == 0xD0 && filtType == cvF_AT) {                   // AT (channel)
                    setDAC(curDac, CVparamCal(d1, curDac)); // chanAT value
                }
                
            }
            if (filtType > 2) // clock out for any filter other than note/velocity
            {
                if (t == 250) {   // Realtime Start Transport
                    clockPulse[curDac] = 1;  // set high at start
                } else if (t == 252) {    // stop
                    setDIG(curDac, LOW);
                }
                else if (t == 248)     // clock
                {
                    setDIG(curDac, clockPulse[curDac]);  // clock!!
                    clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                }
            }
            
            
            /*
            if (curDac < 4)                       // A1-4 jacks get pitch for note on/off events, with gates going to the D1-4 jacks
            {
                if (t == 144)    // note on
                {
                    digitalWriteFast(dig1 + (outp - 36), HIGH);     // GATE on
                    setDAC(curDac, CVnoteCal(d1, curDac));     // send note value to CV, -5 to 5v
                }
                else if (t == 128)      // note off
                {
                    digitalWriteFast(dig1 + (outp - 36), LOW); // GATE off
                }
            }
            else if (curDac == 4)                 // A5 gets velocity, D5 gets clock / 16 that starts/stops with realtime start/stop
            {
                if (t == 250)    // Realtime Start Transport
                {
                    digitalWriteFast(dig5, HIGH);  // pulse on at start
                }
                else if (t == 248 and startCount < 16)     // clock
                {
                    if (startCount < 15)
                    {
                        startCount++;
                    }
                    else if (startCount == 15)
                    {
                        setDIG(curDac, clockPulse[curDac]);  // clock!!
                        clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                        startCount = 0; // was startCount++ EB TODO: Verify this!
                    }
                }
                else if (t == 252)     // Realtime Stop Transport
                {
                    startCount = 0;
                    digitalWriteFast(dig5, LOW);   // pulse off after 16 clocks
                }
                else if (t == 144)     // note on
                {
                    setDAC(curDac, CVparamCal(d2, curDac));   // send velocity to CV5, 0-5v
                }
            }
            else if (curDac == 5)                 // A6 gets mod wheel, D6 gets clock that starts/stops with realtime start/stop
            {
                if (t == 250)    // Realtime Start Transport
                {
                    clockPulse = 1;  // set high at start
                }
                else if (t == 252)     // stop
                {
                    setDIG(curDac, LOW);
                }
                else if (t == 248)     // clock
                {
                    setDIG(curDac, clockPulse[curDac]);  // clock!!
                    clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                }
                else if (t == 176 and d1 == 1)     // mod wheel
                {
                    setDAC(curDac, CVparamCal(d2, curDac));   // send mod wheel to CV6, 0-5v
                }
            }*/
        }
    }
}

bool filtRoute(int t, int ch, int f)   // evaluate MIDI type and ch against filter setting
{
    int filtChan = f >> 3;
    //Serial.print("filtChan:"); Serial.print(filtChan); Serial.print(" ch:"); Serial.println(ch);
    if ((t == 0x80) || (t == 0x90) || (t == 0xA0) || (t == 0xD0) || (t == 0xE0))             // note on, note off, polyAT, chanAT, pitch bend
    {
        if (f & B00000001 && (filtChan == ch || filtChan == 0))
        {
            return true;    // keyboard events routed
        }
    }
    else if ((t == 0xB0) || (t == 0xC0) || (t == 0xF3) || (t == 0xF0) || (t == 0xF7))        // control change, program change, song select, SysEx
    {
        if (f & B00000010 && (filtChan == ch || filtChan == 0))
        {
            return true;    // parameter events routed
        }
    }
    else if (t)                                                                          // common and realtime (transport/clock) events
    {
        if (f & B00000100)
        {
            return true;    // transport/clock events routed
        }
    }
    return false;
}

/*
int16_t filtCV(int t, int16_t d1, int16_t d2, int ch, int f) // filter messages against CV routing
{
    // test channel against filter, if not 0 or no match then exit
    int filtChan = f >> 3;
    if (filtChan != ch || filtChan != 0) {return -1;} // no ch match
    
    int filtType = f & B00000111; // gather current filter setting
    
    if (t == 0x90 && filtType == cvF_NOT) {                         // note on (ignore noteoff)
        setDAC(curDac, CVnoteCal(d1, curDac)); // pitch of the note
    } else if (t == 0x90 && filtType == cvF_VEL) {                  // velocity
        setDAC(curDac, CVparamCal(d2, curDac)); // velocity
    } else if (t == 0xE0 && filtType == cvF_PW) {                   // Pitch Wheel
        setDAC(curDac, CV14bitCal( (d1 << 8 | d2), curDac)); // 14 bit converted to 16 bit value
    } else if (t == 0xB0 && d1 == 1 && filtType == cvF_MOD) {       // Mod Wheel
        setDAC(curDac, CVparamCal(d2, curDac)); // mod value
    } else if (t == 0xB0 && filtType == cvF_CCs) {                  // CCs (any)
        setDAC(curDac, CVparamCal(d2, curDac)); // CC value
    } else if (t == 0xB0 && D1 == 74 && filtType == cvF_CC74) {     // CC74 (MPE Y axis)
        setDAC(curDac, CVparamCal(d2, curDac)); // MPE Y axis value
    } else if (t == 0xA0 && filtType == cvF_AT) {                   // AT (poly, any key)
        setDAC(curDac, CVparamCal(d2, curDac)); // polyAT value, any key
    } else if (t == 0xD0 && filtType == cvF_AT) {                   // AT (channel)
        setDAC(curDac, CVparamCal(d1, curDac)); // chanAT value
    } else {
        return -1; // no match
    }
} */

void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort)
{
    Serial.print("rxSysex: len: ");
    Serial.print(len); Serial.print(" array: ");
    
    for (unsigned int i = 0; i < len; i++)
    {
        Serial.print(sysexarray[i]); Serial.print(", ");
    }
    Serial.print(" inp:"), Serial.println(inPort);
    
    // =======================================================================
    // Before we route the received sysex to an output...
    // =======================================================================
    // ...parse Universal sysex messages that we want to do something with
    if (sysexarray[0] == 240 && sysexarray[1] == 126 && sysexarray[3] == 6 && sysexarray[4] == 2)   // SysExID reply
    {
        #ifdef SDCARD_SUPPORT
        if (sysexarray[5] != 0)
        {
            matchSysExID(sysexarray[5], 0, 0);  // single byte ID
        }
        else
        {
            matchSysExID(0, sysexarray[6], sysexarray[7]);  // three byte ID
        }
        #endif
        
        String rc = mfg;                          // consider optimizing / removing string type
        Serial.print("Rc: "); Serial.println(rc);
        const char * shortName = rc.substring(0, 6).c_str();
        
        MRInputPort *inport = router.inputAt(inPort);
        MROutputPort *outport = router.outputAt(inPort);
        strncpy(inport->name, shortName, 6);
        strncpy(outport->name, shortName, 6);
        //    inputNames[inPort] = rc.substring(0, 6);    // shorten mfg name
        //    outputNames[inPort] = rc.substring(0, 6);
        rdFlag = 1; // flag to redraw screen
    }
    // =======================================================================
    
    // Route to 6 MIDI DIN Ports
    for (int outp = 0; outp < 6; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            dinlist[outp]->sendSysEx(len, sysexarray, true);
        }
    }
    
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            midilist[outp - 6]->sendSysEx(len, sysexarray, true);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            unsigned char sysexarray2[20] = {247, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 48, 16, 64, 0, 1, 0, 0, 23, 247};
            unsigned char SysExLength2 = 20;
            unsigned char cable2 = 0;
            usbMIDI.sendSysEx(SysExLength2, sysexarray2, true, cable2);

            for (unsigned int i = 0; i < len; i++)
            {
                sysexarray2[i] = sysexarray[i];
            }
            
            usbMIDI.send_now();
            Serial.print("sending sysex to computer, len: ");
            Serial.print(len);
            Serial.print(", array: ");
            for (unsigned int i = 0; i < len; i++)
            {
                Serial.print(sysexarray2[i]); Serial.print(", ");
            }
            Serial.println("");
            usbMIDI.sendSysEx(len, sysexarray2, true, outp - 18);
            usbMIDI.send_now();
        }
    }
}

float CVnoteCal(int note, int dac)
{
    float cvrange = (dacPos[dac] - dacNeg[dac]);
    return ((note * (cvrange / 120)) + dacNeg[dac]);
    
}

float CVparamCal(int data, int dac)
{
    float cvrange = ((dacPos[dac] - dacNeg[dac]) / 2);
    return (((data * (cvrange / 127)) + dacNeg[dac]) + cvrange);
}

float CV14bitCal(int16_t data, int dac)
{
    float cvrange = ((dacPos[dac] - dacNeg[dac]));
    return (((data * (cvrange / 16383)) + dacNeg[dac]));
}

void showADC()
{
    Serial.print("ADC1: "); Serial.print(analogRead(adc1)); Serial.print(" ADC2: "); Serial.println(analogRead(adc2));
}

void profileInstruments()
{
    // send Sysex ID requests to all DIN and USB MIDI devices (not to DAW)
    for (int i = 0; i < 6; i++)
    {
        dinlist[i]->sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    }
    updateUSB();
}

void updateUSB()
{
    // gather status
    //Serial.println("updateUSB");
    for (int i = 0; i < 10; i++)
    {
        //Serial.print("U: "); Serial.println(i);
        if (usbWasActive[i] != *midilist[i])
        {
            int tst = *midilist[i];
            //Serial.print("ML: "); Serial.println(tst);
            //Serial.print("Change USB"); Serial.println(i+1);
            
            String prod = (const char *)midilist[i]->product();
            
            if (prod == "") {
                prod = "USB ";
                prod.append(i+1);
            }
            Serial.print("USB Device detected: "); Serial.println(prod);

            char portIndex = (char)(i + 6);
            const char * shortName = prod.substring(0, 9).c_str();
            //const char * medName = prod.substring(0, 9).c_str();
            
            MRInputPort *inport = router.inputAt(portIndex);
            MROutputPort *outport = router.outputAt(portIndex);
            strncpy(inport->name, shortName, 6);
            strncpy(outport->name, shortName, 8);
            
            usbWasActive[i] = tst;
            
            drawColumns();
            drawRows();
        }
    }
}