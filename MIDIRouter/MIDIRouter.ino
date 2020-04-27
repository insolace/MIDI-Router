/*

    MIDI Router
    Created by Eric Bateman (eric at timeline85 dot com)
    http://www.midirouter.com

    MIDIRouter.ino - MIDI Router Arduino Sketch
    Source code written by Eric Bateman with contributions from Kurt Arnlund
    Copyright © 2020 Eric Bateman and Kurt Arnlund. All rights reserved.

    License:GNU General Public License v3.0

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
///
///
/// @file		MIDIRouter.ino
/// @brief		Main sketch
/// Project 	MIDIRouter Library
///
/// @details	Main MIDI Router INO implementation
/// @n @a		Developed with [embedXcode+](https://embedXcode.weebly.com)
///
/// @author		Eric Bateman and Kurt Arnlund
/// @author		Timeline85 / Ingenious Arts and Technologies LLC
/// @date		4/25/20 6:04 PM
/// @version	1.0.0
///
/// @copyright	(c) 2020 Eric Bateman and Kurt Arnlund
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
/// @n
///


// Core library for code-sense - IDE-based
// !!! Help: http://bit.ly/2AdU7cu
#if defined(WIRING) // Wiring specific
#include "Wiring.h"
#elif defined(MAPLE_IDE) // Maple specific
#include "WProgram.h"
#elif defined(ROBOTIS) // Robotis specific
#include "libpandora_types.h"
#include "pandora.h"
#elif defined(MPIDE) // chipKIT specific
#include "WProgram.h"
#elif defined(DIGISPARK) // Digispark specific
#include "Arduino.h"
#elif defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(LITTLEROBOTFRIENDS) // LittleRobotFriends specific
#include "LRF.h"
#elif defined(MICRODUINO) // Microduino specific
#include "Arduino.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(REDBEARLAB) // RedBearLab specific
#include "Arduino.h"
#elif defined(RFDUINO) // RFduino specific
#include "Arduino.h"
#elif defined(SPARK) || defined(PARTICLE) // Particle / Spark specific
#include "application.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

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


// Include application, user and local libraries
// !!! Help http://bit.ly/2CL22Qp
// Teensy - Application Libraries
#include <elapsedMillis.h>
#include <stdint.h>

// Teensy Application Libraries
#include <Wire.h>
#include <SPI.h>
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)
#include <Encoder.h>
#include <Bounce2.h>
#include <EEPROM.h>

// User Libraries
#include "SdFat.h"
#include "GSL1680.h"            // modified firmware, see github.com/insolace
#include "Adafruit_GFX.h"       // vvvvv
#include "Adafruit_RA8875.h"    // both of these are modified, see github.com/insolace
#include "MIDI.h"

#include "MIDIRouter_Library.h"


// Define structures and classes


// Define variables and constants
/// SdFatSdioEX - SD card object
/// @brief [KURT] this will be moving into the MIDI Router library
SdFatSdioEX SD;

/// File - SysCsvFile csv file object
/// @brief [KURT] this will be moving into the MIDI Router library
File SysCsvFile; // create Sysex CSV object
/// CSV_DELIM CSV file field delimiter
#define CSV_DELIM ','

// Knob
#define EncA 26         ///< Encoder pin 1 input
#define EncB 27         ///< Encoder pin 2 input
#define EncSwitch 28    ///< Encoder push button pin input

// DAC 16bit SPI
#define dacA B00010100 ///< DAC 1 address
#define dacB B00010010 ///< DAC 2 address
#define dacC B00010110 ///< DAC 3 address
#define dacD B00010000 ///< DAC 4 address
#define dALL B00110100 ///< DAC ALL address

#define CS 43   ///< SPI chip select pin

// DAC 12bit internal
#define dac5 A22    ///< DAC 5 pin
#define dac6 A21    ///< DAC 6 pin

// DOUT Gate 1-6 setup
#define dig1 3      ///< DOUT 1 pin
#define dig2 4      ///< DOUT 2 pin
#define dig3 5      ///< DOUT 3 pin
#define dig4 6      ///< DOUT 4 pin
#define dig5 22     ///< DOUT 5 pin
#define dig6 21     ///< DOUT 6 pin
#define adc1 A9     ///< Analog to Digital Converter 1 pin
#define adc2 A6     ///< Analog to Digital Converter 2 pin

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
#endif // MIDI_SERIAL_SUPPORT

/*  todo - can't seem to figure out the right class to reference when making this array of pointers.
    Arduino MIDI library is different than the Teensy USB (see below)

    midi::MidiInterface<HardwareSerial> * dinlist[5] = {
    &MIDI1, &MIDI2, &MIDI3, &MIDI4, &MIDI5, &MIDI6
    };
*/

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

// Create MIDI interval timer
IntervalTimer callMIDI; ///< Interval timer for midi io scheduling
#define INTERVALMIDI 250 ///< how often (in microseconds) we call routeMidi()

// TFT Display pins
#define RA8875_INT 15 ///< graphic interrupt
#define RA8875_CS 14 ///<  chip select
#define RA8875_RESET 35 ///< reset

// Initialize graphics
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET); ///< TFT Display Driver Instance

// Touch Pins
#define WAKE 16 ///< wakeup! (is this used?)
#define INTRPT 17 ///< touch interrupt

// touch
GSL1680 TS = GSL1680(); ///< Touch screen driver

// clock stuff
boolean clockPulse = 0; ///< clock pulse boolean
int startCount = 0; ///< clock pulse start count

// Teensy LED
int led = 13; ///< LED pin

// Screen Backlight Intensity
int backLight = 255; ///< backlight intensity

// Knob values
long oldPosition = 0; ///< encoder old position
long newPosition = 0; ///< encoder new position
int knobVal = 0; ///< encoder knob value
int oldKnobVal = 0; ///< encoder old knob value
bool knobDir = 0;  ///< encoder knob direction 0 = CCW, 1 = CW
bool knobAccelEnable = 0; ///< encoder knob acceleration
unsigned long knobTimer = millis(); ///< encoder knob timer
unsigned long knobSlowdown = 2;  ///< wait this many ms before checking the knob value
int knobSpeedup = 3; ///< threshold for difference between old and new value to cause a speed up
float knobSpeedRate = 2.8; ///< factor (exponent) to speed up by
int knobMin = 0; ///< encoder knob min value
int knobMax = 8; ///< encoder knob max value

// DAC stuff
float cvee = 0; ///< [Eric] - document me
long cveeKnobOffset = 0; ///< [Eric] - document me

long dacNeg[6]; ///< [Eric] - document me
long dacPos[6]; ///< [Eric] - document me
long dacOffset[120]; ///< [Eric] - document me

int eeprom_addr_offset = 0; ///< Create address offset so Array2 is located after dacOffset in EEPROM

// Colors
#include "ColorCalc.h"
uint16_t tbColor     = RGBColor(0, 150, 0).asUint16();       ///< tempo/clock box
uint16_t hbColor     = RGBColor(102, 102, 102).asUint16();   ///< setup/home button
uint16_t ibColor     = RGBColor(0, 0, 150).asUint16();       ///< input page box
uint16_t insColor    = RGBColor(0, 0, 100).asUint16();       ///< inputs box color
uint16_t insColFlash = RGBColor(0, 0, 255).asUint16();       ///< input flash color
uint16_t obColor     = RGBColor(150, 0, 0).asUint16();       ///< output page box
uint16_t outsColor   = RGBColor(100, 0, 0).asUint16();       ///< putputs box color
uint16_t outColFlash = RGBColor(255, 0, 0).asUint16();       ///< output flash color
uint16_t gridColor   = RGBColor(102, 102, 102).asUint16();   ///< grid
uint16_t linClr      = RGBColor(0, 0, 0).asUint16();         ///< lines
uint16_t txColor     = RGBColor(255, 255, 255).asUint16();   ///< text
uint16_t routColor   = RGBColor(255, 255, 255).asUint16();   ///< routing
uint16_t actFieldBg  = RGBColor(0, 0, 255).asUint16();       ///< Active Field color
uint16_t fieldBg     = RGBColor(50, 50, 50).asUint16();      ///< inactive field color


uint16_t posCol;  ///< for CV calib
uint16_t negCol;  ///< for CV calib

#define SPEED 4     ///< [Eric] -document me

long fingers = 0,   ///< [Eric] -document me
     curFing = 0,    ///< [Eric] -document me
     x = 0,          ///< [Eric] -document me
     y = 0;          ///< [Eric] -document me

// ============================================================
// variables begin here
// ============================================================

// Screen
int WIDE = 799; ///< [Eric] -document me
int TALL = 479; ///< [Eric] -document me

// Rotation, 0 = normal, 2 = 180
int curRot = 2; ///< [Eric] -document me

// devices
int rows = 6; ///< [Eric] -document me
int columns = 6; ///< [Eric] -document me

// Sysex
uint8_t sysexIDReq[] = {240, 126, 127, 6, 1, 247}; ///< [Eric] -document me

// Menu options
int menu = 0;  ///< which menu are we looking at?  0 = routing, 1 = CV calibration
int actField = 1; ///< which data entry field on the page is active?

boolean rdFlag = 0; ///< flag to redraw screen

// Routing page
int inPages = 6; ///< [Eric] -document me
int outPages = 7; ///< [Eric] -document me
//int devices = pages * 6; ///< [Eric] -document me
int pgOut = 0; ///< [Eric] -document me
int pgIn = 0; ///< [Eric] -document me
//

// timers to flash inputs
elapsedMillis elapseIn; ///< [Eric] -document me
elapsedMillis elapseIn1; ///< [Eric] -document me
elapsedMillis elapseIn2; ///< [Eric] -document me
elapsedMillis elapseIn3; ///< [Eric] -document me
elapsedMillis elapseIn4; ///< [Eric] -document me
elapsedMillis elapseIn5; ///< [Eric] -document me
elapsedMillis elapseIn6; ///< [Eric] -document me

unsigned int flashTime = 1000; ///< delay in milliseconds between activity flashes
int inFlag[5]; ///< [Eric] -document me

// Font color
uint16_t fColor = RA8875_WHITE; ///< [Eric] -document me
uint16_t fBG = 0; ///< [Eric] -document me

// Font dim
int fSize = 3; ///< [Eric] -document me
int fWidth = 18; ///< [Eric] -document me
int fHeight = 25; ///< [Eric] -document me
uint16_t curX = 20; ///< [Eric] -document me
uint16_t curY = 20; ///< [Eric] -document me
int tBord = 5; ///< buffer/border from edge of screen to beginning of text

// =================================
// Routing menu definitions
// =================================

// Rows
int rOffset = 119;  ///< was 152 - [Eric] -document me
int rHeight = (TALL - rOffset) / rows;  ///< 60 -  [Eric] -document me
int tROffset = (rHeight / 2) - (fHeight / 2); ///< text vertical offset in rows

// Columns
int cOffset = 199;  ///< was 238 - [Eric] -document me
int cWidth = (WIDE - cOffset) / columns;  ///< 100 - [Eric] -document me
int tCOffset = (cWidth / 2) - (fHeight / 5); ///< text horizontal offset in rows

// Tempo Box
float tbWidth = cOffset / 2; ///< [Eric] -document me
float tbHeight = rOffset / 2; ///< [Eric] -document me
float  tbOX = (cOffset - tbWidth);    ///< origin X
float  tbOY = (rOffset - tbHeight);  ///< origin Y
int  tbText = 60; ///< [Eric] -document me
int  tempo = 120; ///< [Eric] -document me

// Settings/Home box
float hbWidth = (cOffset - tbWidth); ///< [Eric] -document me
float hbHeight = (rOffset - tbHeight); ///< [Eric] -document me
int hbOX = 0;    ///< origin X
int hbOY = 0;    ///< origin Y

// =================================
// CV Calibration menu definitions
// =================================

int menuCV_butDacNeg5_x = 150,  ///< [Eric] -document me
    menuCV_butDacNeg5_y = rOffset + 100, ///< [Eric] -document me
    menuCV_butDacNeg5_w = 125,  ///< [Eric] -document me
    menuCV_butDacNeg5_h = 50;   ///< [Eric] -document me
int menuCV_butDacPos5_x = 460,  ///< [Eric] -document me
    menuCV_butDacPos5_y = rOffset + 100, ///< [Eric] -document me
    menuCV_butDacPos5_w = 125,  ///< [Eric] -document me
    menuCV_butDacPos5_h = 50;   ///< [Eric] -document me
int CVcalSelect = 0; ///< [Eric] -document me

// touch
char ystr[16];///< [Eric] -document me
char xstr[16];///< [Eric] -document me
long touchX = 0;///< [Eric] -document me
long touchY = 0;///< [Eric] -document me
long lastPress = 0;///< [Eric] -document me
long newX = 0;///< [Eric] -document me
long newY = 0;///< [Eric] -document me
int difX = 0;///< [Eric] -document me
int difY = 0;///< [Eric] -document me

unsigned long touchShort = 300;     ///< (ms) must touch this long to trigger
int tMargin = 5;       ///< pixel margin to filter out duplicate triggers for a single touch

float clearRouting = 0; ///< [Eric] -document me
float pi = 3.141592; ///< [Eric] -document me

int curRoute = 0;  ///< storage for current routing/filter value
int curCol = 0; ///< [Eric] -document me
int curRow = 0; ///< [Eric] -document me

/// Initial routing
/// matrix for routing
/// Each byte in the routing matrix is decoded thusly:
/// -------------------------------------------------
/// bit 0 = keyboard (note on/off, pitchbend, aftertouch, etc)
/// bit 1 = parameters (CC, NRPN/RPN, Sysex parameters etc)
/// bit 3 = transport (clock, start/stop)
/// bit 4 = global channel flag (0 = pass all channels, 1 = filter using bits 5-8)
/// bits 5-8 = channel filter (only pass events matching this channel)
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

// CSV for SD
char syIdHex[20]; ///< [Eric] -document me
char mfg[80]; ///< [Eric] -document me
int16_t idLen; ///< [Eric] -document me
int16_t idB1; ///< [Eric] -document me
int16_t idB2; ///< [Eric] -document me
int16_t idB3; ///< [Eric] -document me


// Prototypes
// !!! Help: http://bit.ly/2TAbgoI
// CalcColor
/// Color Calculation
/// @param r red 0-255
/// @param g green 0-255
/// @param b blue 0-255
/// @return uint16_t 16 bit color
uint16_t newColor(uint8_t r, uint8_t g, uint8_t b);

// EEPROM
void saveEEPROM(); ///< save to eeprom
void loadEEPROM(); ///< load from eeprom

// DAC
/// set DAC output value
/// @param dac DAC identifier
/// @param data 32 bit output value
void setDAC(int dac, uint32_t data);

// TOUCH
void touchIO(); ///< perform touch i/o
void drawTouchPos(); ///< [Eric] -document me
void evaltouch(); ///< [Eric] -document me
void drawMenu_Routing(); ///< [Eric] -document me
void refMenu_Routing(); ///< [Eric] -document me
void refMenu_Calibrate(); ///< [Eric] -document me
void drawMenu_Calibrate(); ///< [Eric] -document me
void drawMenu_Calibrate_udcv(); ///< [Eric] -document me
void readKnob(); ///< [Eric] -document me
void knobZero(); ///< [Eric] -document me
void knobFull(); ///< [Eric] -document me
/// @param v  [Eric] = comment on what V is
void knobSet(int v);
void knob_calCV(); ///< [Eric] -document me
/// @param x x location
/// @param y y location
/// @param bx box x location
/// @param by box y location
/// @param bw box width
/// @param bh box height
/// @return boolean true if x,y location is inside the box
boolean withinBox(int x, int y, int bx, int by, int bw, int bh);
/// using location x determine what column it is in
/// @param x location x
/// @return column
int getTouchCol(long x);
/// using location x determine what row it is in
/// @param y location y
/// @return row
int getTouchRow(long y);

// DRAW
void drawBox(); ///< [Eric] -document me
void drawColumns(); ///< [Eric] -document me
void drawRows(); ///< [Eric] -document me
void drawRouting(); ///< [Eric] -document me
void drawGLines(); ///< [Eric] -document me
void drawBGs(); ///< [Eric] -document me
void drawHomeScreen(); ///< [Eric] -document me
/// [Eric] -document me
/// @param c  [Eric] -document me
/// @param r [Eric] -document me
void drawPiano(int c, int r);
/// [Eric] -document me
/// @param inp [Eric] -document me
/// @param state [Eric] -document me
void flashIn(int inp, int state);
/// [Eric] -document me
/// @param filename [Eric] -document me
/// @param x [Eric] -document me
/// @param y [Eric] -document me
void bmpDraw(const char *filename, int x, int y);
/// [Eric] -document me
/// @param f [Eric] -document me
/// @return uint16_t 16 bit value
uint16_t read16(File &f);
/// [Eric] -document me
/// @param f [Eric] -document me
/// @return uint32_t 32 bit value
uint32_t read32(File &f);
/// create a 565 color
/// @param r red 0-255
/// @param g green 0-255
/// @param b blue 0-255
/// @return uint16_t 16 bit 565 color value
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

// MIDI
/// [Eric] -document me
void routeMidi();
/// [Eric] -document me
/// @param t [Eric] -document me
/// @param d1 [Eric] -document me
/// @param d2 [Eric] -document me
/// @param ch [Eric] -document me
/// @param inPort [Eric] -document me
void transmitMIDI(int t, int d1, int d2, int ch, byte inPort);
/// [Eric] -document me
/// @param len [Eric] -document me
/// @param sysexarray [Eric] -document me
/// @param inPort [Eric] -document me
void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort);
/// [Eric] -document me
/// @param note [Eric] -document me
/// @param dac [Eric] -document me
/// @return float value
float CVnoteCal(int note, int dac);
/// [Eric] -document me
/// @param data [Eric] -document me
/// @param dac [Eric] -document me
/// @return float value
float CVparamCal(int data, int dac);
/// [Eric] -document me
void showADC();
/// [Eric] -document me
void profileInstruments();
/// [Eric] -document me
/// @param t [Eric] -document me
/// @param f [Eric] -document me
/// @return bool true/false
bool filtRoute(int t, int f);

// UTIL
/// matchSysExID
/// @param b1 [Eric] -document me
/// @param b2 [Eric] -document me
/// @param b3 [Eric] -document me
void matchSysExID(int16_t b1, int16_t b2, int16_t b3);
/// [Eric] -document me
void printMatch();
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param str [Eric] -document me
/// @param size [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadText(File* SysCsvFile, char* str, size_t size, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadInt32(File* SysCsvFile, int32_t* num, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadInt16(File* SysCsvFile, int16_t* num, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadDouble(File* SysCsvFile, double* num, char delim);
/// [Eric] -document me
/// @param SysCsvFile [Eric] -document me
/// @param num [Eric] -document me
/// @param delim [Eric] -document me
/// @return int value
int csvReadFloat(File* SysCsvFile, float* num, char delim);
/// Close CSV File
void csvClose();
/// [Eric] -document me
/// @param r [Eric] -document me
/// @return int value
int reOrderR(int r);

// TXT
/// debug print
/// @param s [Eric] -document me
/// @param sz [Eric] -document me
void dPrint(String s, int sz = fSize);
/// debug write
/// @param c [Eric] -document me
/// @param s [Eric] -document me
void dWrite(unsigned char c, unsigned int s);


// Utilities
USING_NAMESPACE_MIDIROUTER
MIDIRouter_Lib router = MIDIRouter_Lib(); ///< Create MIDI Router Object

// Functions


///! Add setup code
void setup()
{
    Serial.begin(115200);
    //  delay(1000);
    if (!SD.begin())
    {
        Serial.println("SD initialization failed!");
        //return;
    }
    else
    {
        Serial.println("SD initialization done.");
    }

    // knob
    router.SetupEncoder(EncA, EncB, EncSwitch);

    // ============================================================
    // EEPROM
    // ============================================================
    loadEEPROM();

    // ============================================================
    // DAC 16bit SPI setup
    // ============================================================

    // set the CS as an output:
    pinMode(CS, OUTPUT);
    digitalWrite(CS, LOW);

    // initialise SPI:
    SPI2.begin();
    SPI2.setBitOrder(MSBFIRST);

    //enable internal ref
    digitalWriteFast(CS, LOW);
    int c1 = B00000001;
    int c2 = B00010000;
    int c3 = B00000000;
    SPI2.transfer(c1);
    SPI2.transfer(c2);
    SPI2.transfer(c3);
    digitalWriteFast(CS, HIGH);

    setDAC(6, dacNeg[0]);

    // ============================================================
    // DAC 12bit internal setup
    // ============================================================

    pinMode(dac5, OUTPUT);
    pinMode(dac6, OUTPUT);
    analogWriteResolution(12);

    // ============================================================
    // D 1-6 setup
    // ============================================================
    pinMode(dig1, OUTPUT);
    pinMode(dig2, OUTPUT);
    pinMode(dig3, OUTPUT);
    pinMode(dig4, OUTPUT);
    pinMode(dig5, OUTPUT);
    pinMode(dig6, OUTPUT);
    pinMode(dig1, OUTPUT);
    //pinMode(adc1, INPUT);  // hardware is there but not implemented yet in software
    //pinMode(adc2, INPUT);


    // ============================================================
    // MIDI setup
    // ============================================================
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

    // USB Devices hosted by TEENSY
    // Wait 1.5 seconds before turning on TEENSY USB Host.  If connected USB devices
    // use too much power, Teensy at least completes USB enumeration, which
    // makes isolating the power issue easier.
    delay(150);
    myusb.begin();

    // ============================================================
    // Touchscreen setup
    // ============================================================
#ifdef TFT_DISPLAY
    // Setup Touch
    TS.begin(WAKE, INTRPT);                 // Startup sequence CONTROLER part
    pinMode(led, OUTPUT);

    //Serial.println("RA8875 start");
    while (!Serial && (millis() <= 1000));

    // Initialise the display
    while (!tft.begin(RA8875_800x480))
    {
        Serial.println("RA8875 Not Found!");
        delay(100);
    }
    Serial.println("Found RA8875");

    tft.displayOn(true);
    tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
    tft.PWM1out(255);

    // Rotation
    tft.setRotation(curRot);

    // With hardware accelleration this is instant
    tft.graphicsMode();
    tft.fillScreen(RA8875_BLACK);
#ifdef STARTUP_PICTURE
    bmpDraw("WELCOME.BMP", 0, 0);
    delay(1000);
    tft.fillScreen(RA8875_BLACK);
#endif //STARTUP_PICTURE

    tft.touchEnable(false);

    randomSeed(analogRead(0));
    tft.textMode();
    tft.textEnlarge(2);
#endif // TFT_DISPLAY
    // end touchscreen setup

#if defined(MIDI_SERIAL_SUPPORT) || defined(MIDI_USB_SUPPORT)
    // ============================================================
    // MIDI interrupt timer
    // ============================================================
    callMIDI.begin(routeMidi, INTERVALMIDI);
#endif // defined(MIDI_SERIAL_SUPPORT) || defined(MIDI_USB_SUPPORT)

    // ============================================================
    // Sysex CSV stuff
    // ============================================================

#ifdef SDCARD_SUPPORT
    if (!SD.chdir("sysexids"))
    {
        Serial.println("chdir failed for Folder1.\n");
    }

    // Open the file.
    SysCsvFile = SD.open("sysexids.csv", FILE_WRITE);
    if (!SysCsvFile)
    {
        Serial.println("open failed");
        return;
    }
#endif // SDCARD_SUPPORT

    // sysex id req
    delay(250);  // allow send/receive buffers to settle, some MIDI devices are chatty when powered on
    profileInstruments();
    delay(250);  // wait for responses

#ifdef TFT_DISPLAY
    // draw homescreen
    drawHomeScreen();
    rdFlag = 0;
#endif // TFT_DISPLAY
}

///! Add loop code
void loop()
{
    //routeMidi(); // check incoming MIDi and route it
    if (rdFlag == 1)
    {
        rdFlag = 0;
        drawHomeScreen();
    }
    readKnob();  // check knob for turn/push
    touchIO();   // process touchscreen input


    //**************************

    // test for active input flashes, turn off after interval
    /*  for (int i = 0; i < 6; i++){
        if (inFlag[i] = 1) {
        if ((elapseIn) > flashTime) {
        inFlag[i] = 0;
        flashIn(i, 0);
        }
        }
        }
    */
}

#include "MR_TOUCH.h"
#include "MR_DRAW.h"
#include "MR_MIDI.h"
#include "MR_UTIL.h"
#include "MR_DAC.h"
#include "MR_TXT.h"
#include "MR_EEPROM.h"
