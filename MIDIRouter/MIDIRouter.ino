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
#include "RA8875.h"     
#include "MIDI.h"

#include "MIDIRouter_Library.h"


// Define structures and classes

// Class that describes a GUI element
class guiElem
{
public:
    /// x/y origin and width/height of element on layer 2
    uint16_t x, y, w, h;

    guiElem (uint16_t, uint16_t, uint16_t, uint16_t);
    void draw(uint16_t destX, uint16_t destY, bool transp);
};

/// Constructor
/// @param a x origin of element
/// @param b y origin of element
/// @param c width of element
/// @param d height of element
guiElem::guiElem(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    x = a; y = b; w = c; h = d;
}



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
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);

// Touch Pins
#define WAKE 16 ///< wakeup! (is this used?)
#define INTRPT 17 ///< touch interrupt

// touch
GSL1680 TS = GSL1680(); ///< Touch screen driver

// ============================================================
// variables begin here
// ============================================================

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
long dacNeg[6]; ///< Calibrated value for -5VDC at dac output
long dacPos[6]; ///< Calibrated value for +5VDC at dac output
long dacOffset[120]; ///< Stored array for custom offsets per note (not yet implemented)

uint16_t posCol;  // for CV calib
uint16_t negCol;  // for CV calib

// EEPROM
int eeprom_addr_offset = 0; ///< Create address offset so Array2 is located after dacOffset in EEPROM

// =====================================================
// Touchscreen and GUI elements begin here
// =====================================================

// GUI Elements read from BMP on SD card

// Utility
guiElem empty (0, 0, 0, 0);
// backgrounds
guiElem bg_homebox (0, 0, 200, 121);
guiElem bg_inputs (200, 0, 100, 121);
guiElem bg_outputs (0, 121, 201, 60);

// routing grid
guiElem grid_routed (201, 241, 98, 60);
guiElem grid_unrouted (201, 121, 100, 60);
guiElem grid_notes (201, 326, 50, 32);
guiElem grid_param (201, 300, 50, 26);
guiElem grid_trans (251, 300, 48, 26);

// icons
guiElem ic_routing (0, 240, 198, 119);
guiElem ic_calib_back (0, 360, 98, 60);
guiElem ic_calib_clear (101, 360, 98, 60);
guiElem ic_din (0, 181, 62, 60);
guiElem ic_usb (69, 181, 62, 60);
guiElem ic_daw (139, 181, 62, 60);
guiElem ic_eur (219, 181, 62, 58);


// Colors
#include "ColorCalc.h"
uint16_t tbColor     = tft.Color565(0, 150, 0);       // tempo/clock box
uint16_t hbColor     = tft.Color565(102, 102, 102);   // setup/home button
uint16_t ibColor     = tft.Color565(0, 0, 150);       // input page box
uint16_t insColor    = tft.Color565(0, 0, 100);       // inputs box color
uint16_t insColFlash = tft.Color565(0, 0, 255);       // input flash color
uint16_t obColor     = tft.Color565(150, 0, 0);       // output page box
uint16_t outsColor   = tft.Color565(100, 0, 0);       // putputs box color
uint16_t outColFlash = tft.Color565(255, 0, 0);       // output flash color
uint16_t gridColor   = tft.Color565(102, 102, 102);   // grid
uint16_t linClr      = tft.Color565(0, 0, 0);         // lines
uint16_t txColor     = tft.Color565(255, 255, 255);   // text
uint16_t routColor   = tft.Color565(255, 255, 255);   // routing
uint16_t actFieldBg  = tft.Color565(0, 0, 255);       // Active Field color
uint16_t fieldBg     = tft.Color565(50, 50, 50);      // inactive field color
uint16_t transp      = tft.Color565(36, 0, 0);         // Transparent key color

// Screen
int WIDE = 799; ///< Width of touchscreen (0-799)
int TALL = 479; ///< Height of touchscreen (0-479)

// Rotation, 0 = normal, 2 = 180
int curRot = 2; ///< Current screen rotation (2 = 180 degrees);

// devices
int rows = 6; ///< Number of rows in routing UI
int columns = 6; ///< Number of columns in routing UI

// Menu options
int menu = 0;  ///< which menu are we looking at?  0 = routing, 1 = CV calibration
int actField = 1; ///< which data entry field on the page is active?

boolean rdFlag = 0; ///< flag to redraw screen

// Routing page
int inPages = 6; ///< Number of input pages in routing UI
int outPages = 7; ///< Number of output pages in routing UI

int pgOut = 0; ///< Current output page displayed in routing UI
int pgIn = 0; ///< Current input page displayed in routing UI

// timers to flash inputs
elapsedMillis elapseIn; ///< Timer for flashing inputs
elapsedMillis elapseIn1; ///< Timer for flashing input column 1
elapsedMillis elapseIn2; ///< Timer for flashing input column 2
elapsedMillis elapseIn3; ///< Timer for flashing input column 3
elapsedMillis elapseIn4; ///< Timer for flashing input column 4
elapsedMillis elapseIn5; ///< Timer for flashing input column 5
elapsedMillis elapseIn6; ///< Timer for flashing input column 6

unsigned int flashTime = 1000; ///< delay in milliseconds between activity flashes
int inFlag[5]; ///< Flag to flash an input column

// Font color
uint16_t fColor = RA8875_WHITE; ///< Font Color
uint16_t fBG = 0; ///< Font Backgeround Color

// Font dim
int fSize = 3; ///< Font Size
int fWidth = 18; ///< Font Width
int fHeight = 25; ///< Font Height
uint16_t curX = 20; ///< Current X coordinate to draw to
uint16_t curY = 20; ///< Current Y coordinate to draw to
int tBord = 5; ///< buffer/border from edge of screen to beginning of text

// =================================
// Routing menu graphic definitions
// =================================

// Rows
int rOffset = 119;  ///< was 152 - Offset from top of screen to first row
int rHeight = (TALL - rOffset) / rows;  ///< 60 -  Row height
int tROffset = (rHeight / 2) - (fHeight / 2); ///< text vertical offset for rows

// Columns
int cOffset = 199;  ///< was 238 - Offset from left side of screen to first column
int cWidth = (WIDE - cOffset) / columns;  ///< 100 - Column width
int tCOffset = (cWidth / 2) - (fHeight / 5); ///< text horizontal offset for columns

// Tempo Box
float tbWidth = cOffset / 2; ///< Tempo box width
float tbHeight = rOffset / 2; ///< Tempo box height
float  tbOX = (cOffset - tbWidth);    ///< Tempo box origin X
float  tbOY = (rOffset - tbHeight);  ///< Tempo box origin Y
int  tempo = 120; ///< Current tempo - not implemented

// Settings/Home box
float hbWidth = (cOffset - tbWidth); ///< Home box width
float hbHeight = (rOffset - tbHeight); ///< Home box height
int hbOX = 0;    ///< Home box origin X
int hbOY = 0;    ///< Home box origin Y

// CV Calibration menu definitions
int menuCV_butDacNeg5_x = 150,  ///< X coordinate for -5VDC calibration box
    menuCV_butDacNeg5_y = rOffset + 100, ///< Y coordinate for -5V calibration box
    menuCV_butDacNeg5_w = 125,  ///< Width for -5V calibration box
    menuCV_butDacNeg5_h = 50;   ///< Height for -5V calibration box
int menuCV_butDacPos5_x = 460,  ///< X coordinate for +5VDC calibration box
    menuCV_butDacPos5_y = rOffset + 100, ///< Y coordinate for +5VDC calibration box
    menuCV_butDacPos5_w = 125,  ///< Width for +5VDC calibration box
    menuCV_butDacPos5_h = 50;   ///< Height for +5VDC calibration box
int CVcalSelect = 0; ///< Current CV/DAC output selected for calibration

// touch
char ystr[16];///< Array to convert touchY from a string for debugging
char xstr[16];///< Array to convert touchX from a string for debugging
long touchX = 0;///< X coordinate of last touchpoint
long touchY = 0;///< Y coordinate of last touchpoint
long lastPress = 0;///< Timer for the last time we received a touch interrupt. Filters out duplicate events.
long newX = 0;///< X coordinate of newly received touchpoint
long newY = 0;///< Y coordinate of newly received touchpoint
int difX = 0;///< The difference between the last touch X coordinate and the one just received. Filters out duplicate events.
int difY = 0;///< The difference between the last touch Y coordinate and the one just received. Filters out duplicate events.

unsigned long touchShort = 300;     ///< (ms) must touch this long to trigger
int tMargin = 5;       ///< pixel margin to filter out duplicate triggers for a single touch

long fingers, curFing, x, y;

// =====================================================
// Routing
float clearRouting = 0; ///< Flag to clear all routings
float pi = 3.141592; ///< Store this in EEPROM to indicate that this isn't the first time we've turned on the MIDI Router. If EEPROM doesn't contain this value, write Zero's to all routing points (EEPROM factory default values are -1)

int curRoute = 0;  ///< Storage for current routing/filter value
int curCol = 0; ///< Current selected column
int curRow = 0; ///< Current selected row

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

// Sysex
uint8_t sysexIDReq[] = {240, 126, 127, 6, 1, 247}; ///< SysEx ID Request

// Variables for storing and reading SysEx/CSV from SD card
char syIdHex[20]; ///< Column 1, SysEx ID header in HEX
char mfg[80]; ///< Column 2, manufacturer name (text/ASCII)
int16_t idLen; ///< Column 3, the length of the SysEx ID in bytes
int16_t idB1; ///< Column 4, ID byte 1
int16_t idB2; ///< Column 5, ID byte 2
int16_t idB3; ///< Column 6, ID byte 3

// =====================================================
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
/// @param data 16 bit output value for DAC1-4, 12 bit output value for DAC5-6
void setDAC(int dac, uint32_t data);

// TOUCH
void touchIO(); ///< perform touch i/o
void drawTouchPos(); ///< Display current touch position on the screen
void evaltouch(); ///< Evaluate current touch coordinates
void drawMenu_Routing(); ///< Draw the routing menu/page
void refMenu_Routing(); ///< Refresh routing menu/page
void refMenu_Calibrate(); ///< Refresh calibration menu/page
void drawMenu_Calibrate(); ///< Draw the calibration menu
void drawMenu_Calibrate_udcv(); ///< Update CV calibration values
void readKnob(); ///< Process knob input
void knobZero(); ///< Zero out the knob value
void knobFull(); ///< Set knob value to max
/// @param v  Value to set knob to
void knobSet(int v);
void knob_calCV(); ///< Process knob input in CV calibration menu/page
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
void drawBox(); ///< Draw the four boxes in the upper left of the display
void drawColumns(); ///< Draw columns
void drawRows(); ///< Draw rows
void drawRouting(); ///< Draw the current routing grid
/// Draw the current routing point
/// @param c  Column
/// @param r Row
void drawRoute(int c, int r);
void blankSelect(); ///< Blank the last selection
void drawGLines(); ///< Draw the routing grid lines
void drawBGs(); ///< Draw backgrounds
void drawHomeScreen(); ///< Draw the home screen
/// Draw a piano at the given row/column routing point
/// @param c  Column
/// @param r Row
void drawPiano(int c, int r);
/// Flash an input column
/// @param inp Input Column
/// @param state State of flashing input
void flashIn(int inp, int state);
/// Draw a bitmap
/// @param filename Filename on the SD card
/// @param x X coordinate to draw the BMP
/// @param y Y coordinate to draw the BMP
void bmpDraw(const char *filename, int x, int y);
/// Read a 16 bit value from the current open file
/// @param f File location
/// @return uint16_t 16 bit value
uint16_t read16(File &f);
/// Read a 32 bit value from the current open file
/// @param f File location
/// @return uint32_t 32 bit value
uint32_t read32(File &f);
/// create a 565 color
/// @param r red 0-255
/// @param g green 0-255
/// @param b blue 0-255
/// @return uint16_t 16 bit 565 color value
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
/// Draw an array
/// @param PImage  name/address of the array
/// @param x X coordinate to draw the array
/// @param y Y coordinate to draw the array
// void drawArray(uint16_t x, uint16_t y);

// MIDI
/// Route the MIDI
void routeMidi();
/// Transmit MIDI
/// @param t Type (noteon, CC, sysex, etc.)
/// @param d1 Data byte 1
/// @param d2 Data byte 2
/// @param ch Channel
/// @param inPort Input port (DIN1, USB, DAW etc)
void transmitMIDI(int t, int d1, int d2, int ch, byte inPort);
/// Transmit SysEx
/// @param len Length of the SysEx array to transmit
/// @param sysexarray Sysex Array
/// @param inPort Input port (DIN, USB, DAW etc)
void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort);
/// Set DAC/CV output to a calibrated 1v/oct
/// @param note MIDI note (0-119, 10 octaves over 10v)
/// @param dac DAC output to set (1-6)
/// @return float value
float CVnoteCal(int note, int dac);
/// Set DAC/CV to a calibrated CC value (0-127)
/// @param data Data to send (0-127)
/// @param dac DAC output to set
/// @return float value
float CVparamCal(int data, int dac);
/// Print ADC values to Serial port
void showADC();
/// Profile instruments connected to DIN ports using SysEx ID Request
void profileInstruments();
/// Evaluate MIDI type against filter
/// @param t MIDI Type (noteon, cc, etc)
/// @param f Filter value
/// @return bool true/false
bool filtRoute(int t, int f);

// UTIL
/// matchSysExID
/// @param b1 Byte 1
/// @param b2 Byte 2
/// @param b3 Byte 3
void matchSysExID(int16_t b1, int16_t b2, int16_t b3);
/// Debugging - print the matched SysEx ID to the Serial port
void printMatch();
/// Read text from CSV
/// @param SysCsvFile File
/// @param str Pointer to store/return text
/// @param size Size
/// @param delim Character separator (",")
/// @return int value
int csvReadText(File* SysCsvFile, char* str, size_t size, char delim);
/// Read 32 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadInt32(File* SysCsvFile, int32_t* num, char delim);
/// Read 16 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadInt16(File* SysCsvFile, int16_t* num, char delim);
/// Read uint 32 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim);
/// Read uint 16 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim);
/// Read double value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadDouble(File* SysCsvFile, double* num, char delim);
/// Read floating point value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadFloat(File* SysCsvFile, float* num, char delim);
/// Close CSV File
void csvClose();
/// Re-order the filtering options so that the most convinient filters come up first
/// @param r Value to be re-ordered
/// @return int value
int reOrderR(int r);

// TXT
/// Print string to display
/// @param s String to print
/// @param sz Font size
void dPrint(String s, int sz = fSize);
/// Print character to display
/// @param c Character
/// @param s Font size
void dWrite(unsigned char c, unsigned int s);
void printVert(String s);

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
    tft.begin(RA8875_800x480);
    if (tft.errorCode() != 0)  {
        Serial.println("RA8875 Not Found!");
        delay(100);
    } else {
    Serial.println("Found RA8875");
    }

    //tft.displayOn(true);
    //tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
    //tft.backlight(1);     // backlight on
    //tft.brightness(255);
    
    // Rotation
    tft.setRotation(curRot);
    
    // External fonts
    tft.setExternalFontRom(ER3304_1, ASCII);
    tft.setFont(EXTFONT);//enable external ROM
    tft.setExtFontFamily(ARIAL);
    tft.setFontSize(X16);
    tft.setFontScale(0);
    tft.setTextColor(txColor);
    
    // With hardware accelleration this is instant
    //tft.graphicsMode();
    tft.clearScreen(RA8875_BLACK);

#ifdef STARTUP_PICTURE

    bmpDraw("WELCOME.BMP", 0, 0);
    delay(2000);
    tft.clearScreen(RA8875_BLACK);
    
#endif //STARTUP_PICTURE
    
    tft.layerEffect(LAYER1); // layer 1, 8bit color
    tft.writeTo(L2);
    bmpDraw("Skins/default/skin_default.bmp", 0, 0);
    tft.writeTo(L1); // layer 1
    
    randomSeed(analogRead(0));

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
