///
/// @file		MIDIRouter.ino
/// @brief		Main sketch
/// Project 	MIDIRouter Library
///
/// @details	<#details#>
/// @n @a		Developed with [embedXcode+](https://embedXcode.weebly.com)
///
/// @author		Kurt Arnlund
/// @author		Isolace / Ingenious Arts and Technologies LLC
/// @date		4/25/20 6:04 PM
/// @version	<#version#>
///
/// @copyright	(c) Kurt Arnlund, 2020
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
#define MIDI_SERIAL_SUPPORT
#define MIDI_USB_SUPPORT
#define SDCARD_SUPPORT
#define TFT_DISPLAY
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
SdFatSdioEX SD;
File SysCsvFile; // create Sysex CSV object
#define CSV_DELIM ','

// Knob
#define EncA 26
#define EncB 27
#define EncSwitch 28
//Encoder myEnc(EncA, EncB);
//Bounce encPush = Bounce();

// DAC 16bit SPI
#define dacA B00010100 // cv 1
#define dacB B00010010
#define dacC B00010110
#define dacD B00010000
#define dALL B00110100

#define CS 43 // chip select

// DAC 12bit internal
#define dac5 A22
#define dac6 A21

// D1-6 setup
#define dig1 3  // teensy pins
#define dig2 4
#define dig3 5
#define dig4 6
#define dig5 22
#define dig6 21
#define adc1 A9
#define adc2 A6

// Hardware and USB Device MIDI
// https://github.com/FortySevenEffects/arduino_midi_library
// Create the Serial MIDI ports
// MIDI_CREATE_INSTANCE(Type, SerialPort, Name)
#ifdef MIDI_SERIAL_SUPPORT
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6);
#endif // MIDI_SERIAL_SUPPORT

/* todo - can't seem to figure out the right class to reference when making this array of pointers.
 Arduino MIDI library is different than the Teensy USB (see below)
 
 midi::MidiInterface<HardwareSerial> * dinlist[5] = {
 &MIDI1, &MIDI2, &MIDI3, &MIDI4, &MIDI5, &MIDI6
 };
 */

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
USBHub hub4(myusb);
#ifdef MIDI_USB_SUPPORT
MIDIDevice midi01(myusb);
MIDIDevice midi02(myusb);
MIDIDevice midi03(myusb);
MIDIDevice midi04(myusb);
MIDIDevice midi05(myusb);
MIDIDevice midi06(myusb);
MIDIDevice midi07(myusb);
MIDIDevice midi08(myusb);
MIDIDevice midi09(myusb);
MIDIDevice midi10(myusb);
MIDIDevice * midilist[10] = {
    &midi01, &midi02, &midi03, &midi04, &midi05, &midi06, &midi07, &midi08, &midi09, &midi10
};
#endif // MIDI_USB_SUPPORT

// Create MIDI interval timer
IntervalTimer callMIDI;
#define INTERVALMIDI 250 // how often (in microseconds) we call routeMidi()

// TFT Display pins
#define RA8875_INT 15 // graphic interrupt
#define RA8875_CS 14 //  chip select
#define RA8875_RESET 35 // reset

// Initialize graphics
Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RESET);

// Touch Pins
#define WAKE 16 // wakeup! (is this used?)
#define INTRPT 17 // touch interrupt

// touch
GSL1680 TS = GSL1680();

// clock stuff
boolean clockPulse = 0;
int startCount = 0;

// Teensy LED
int led = 13;

// Screen Backlight Intensity
int backLight = 255;

// Knob values
long oldPosition = 0;
long newPosition = 0;
int knobVal = 0;
int oldKnobVal = 0;
unsigned long knobTimer = millis();
unsigned long knobSlowdown = 2;  // wait this many ms before checking the knob value
int knobSpeedup = 3; // threshold for difference between old and new value to cause a speed up
float knobSpeedRate = 2.8; // factor (exponent) to speed up by
int knobMin = 0;
int knobMax = 1024;

// DAC stuff
float cvee = 0;
long cveeKnobOffset = 0;

long dacNeg[6];
long dacPos[6];
long dacOffset[120];


//Create address offset so Array2 is located after dacOffset in EEPROM
int eeprom_addr_offset = 0;

// Colors
#include "ColorCalc.h"
uint16_t tbColor     = RGBColor(0, 150, 0).asUint16();       // tempo/clock box
uint16_t hbColor     = RGBColor(102, 102, 102).asUint16();   // setup/home button
uint16_t ibColor     = RGBColor(0, 0, 150).asUint16();       // input page box
uint16_t insColor    = RGBColor(0, 0, 100).asUint16();       // inputs box color
uint16_t insColFlash = RGBColor(0, 0, 255).asUint16();       // input flash color
uint16_t obColor     = RGBColor(150, 0, 0).asUint16();       // output page box
uint16_t outsColor   = RGBColor(100, 0, 0).asUint16();       // putputs box color
uint16_t outColFlash = RGBColor(255, 0, 0).asUint16();       // output flash color
uint16_t gridColor   = RGBColor(102, 102, 102).asUint16();   // grid
uint16_t linClr      = RGBColor(0, 0, 0).asUint16();         // lines
uint16_t txColor     = RGBColor(255, 255, 255).asUint16();   // text
uint16_t routColor   = RGBColor(255, 255, 255).asUint16();   // routing
uint16_t actFieldBg  = RGBColor(0, 0, 255).asUint16();       // Active Field color
uint16_t fieldBg     = RGBColor(50, 50, 50).asUint16();      // inactive field color


uint16_t posCol;  // for CV calib
uint16_t negCol;  // for CV calib

#define SPEED 4

long fingers, curFing, x, y;

// ============================================================
// variables begin here
// ============================================================

// Screen
int WIDE = 799;
int TALL = 479;

// Rotation, 0 = normal, 2 = 180
int curRot = 2;

// devices
int rows = 6;
int columns = 6;

// Sysex
uint8_t sysexIDReq[] = {240, 126, 127, 6, 1, 247};

// Menu options
int menu = 0;  // which menu are we looking at?  0 = routing, 1 = CV calibration
int actField = 1; // which data entry field on the page is active?

boolean rdFlag = 0; // flag to redraw screen

// Routing page
int inPages = 6;
int outPages = 7;
//int devices = pages * 6;
int pgOut = 0;
int pgIn = 0;
//

// timers to flash inputs
elapsedMillis elapseIn;
elapsedMillis elapseIn1;
elapsedMillis elapseIn2;
elapsedMillis elapseIn3;
elapsedMillis elapseIn4;
elapsedMillis elapseIn5;
elapsedMillis elapseIn6;
// delay in milliseconds between activity flashes
unsigned int flashTime = 1000;
int inFlag[5];

// Font (deprecated?)
int tSize = 16;

// Font dim
int fSize = 3;
int fWidth = 18;
int fHeight = 25;
uint16_t curX = 20;
uint16_t curY = 20;
int tBord = 5; // buffer/border from edge of screen to beginning of text

// =================================
// Routing menu definitions
// =================================

// Rows
int rOffset = 152;
int rHeight = (TALL - rOffset) / rows;  // 40
int tROffset = (rHeight/2)-(fHeight/2);  // text vertical offset in rows
int curGrid = 0;
// Columns
int cOffset = 238;
int cWidth = (WIDE - cOffset) / columns;  // 60
int tCOffset = (cWidth/2)-(fHeight/5);  // text horizontal offset in rows

// Tempo Box
float tbWidth = cOffset/2;
float tbHeight = rOffset/2;
float  tbOX = (cOffset - tbWidth);    // origin X
float  tbOY = (rOffset - tbHeight);  // origin Y
int  tbText = 60;
int  tempo = 120;

// Settings/Home box
float hbWidth = (cOffset - tbWidth);
float hbHeight = (rOffset - tbHeight);
int hbOX = 0;    // origin X
int hbOY = 0;    // origin Y

// =================================
// CV Calibration menu definitions
// =================================

int menuCV_butDacNeg5_x = 150 , menuCV_butDacNeg5_y = rOffset+100, menuCV_butDacNeg5_w = 125, menuCV_butDacNeg5_h = 50;
int menuCV_butDacPos5_x = 460 , menuCV_butDacPos5_y = rOffset+100, menuCV_butDacPos5_w = 125, menuCV_butDacPos5_h = 50;
int CVcalSelect = 0;

// touch
char ystr[16];
char xstr[16];
long touchX = 0;
long touchY = 0;
long lastPress = 0;
long newX = 0;
long newY = 0;
int difX = 0;
int difY = 0;

unsigned long touchShort = 300;     // (ms) must touch this long to trigger
int tMargin = 5;       // pixel margin to filter out duplicate triggers for a single touch

float clearRouting = 0;
float pi = 3.141592;

// Initial routing
// matrix for routing
bool routing[50][50] = {  // [input port][output port]
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
    
};

// CSV for SD
char syIdHex[20];
char mfg[80];
int16_t idLen;
int16_t idB1;
int16_t idB2;
int16_t idB3;


// Prototypes
// !!! Help: http://bit.ly/2TAbgoI
// CalcColor
uint16_t newColor(uint8_t r, uint8_t g, uint8_t b);

// EEPROM
void saveEEPROM();
void loadEEPROM();

// DAC
void setDAC(int dac, uint32_t data);

// TOUCH
void touchIO();
void drawTouchPos();
void evaltouch();
void drawMenu_Routing();
void refMenu_Routing();
void refMenu_Calibrate();
void drawMenu_Calibrate();
void drawMenu_Calibrate_udcv();
void readKnob();
void knob_calCV();
boolean withinBox(int x, int y, int bx, int by, int bw, int bh);
int getTouchCol(long x);
int getTouchRow(long y);

// DRAW
void drawBox();
void drawColumns();
void drawRows();
void drawRouting();
void drawGLines();
void drawBGs();
void drawHomeScreen();
void flashIn(int inp, int state);
void bmpDraw(const char *filename, int x, int y);
uint16_t read16(File& f);
uint32_t read32(File& f);
uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

// MIDI
void routeMidi();
void transmitMIDI(int t, int d1, int d2, int ch, byte inPort);
void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort);
float CVnoteCal(int note, int dac);
float CVparamCal(int data, int dac);
void showADC();
void profileInstruments();

// UTIL
void matchSysExID(int16_t b1, int16_t b2, int16_t b3);
void printMatch();
int csvReadText(File* SysCsvFile, char* str, size_t size, char delim);
int csvReadInt32(File* SysCsvFile, int32_t* num, char delim);
int csvReadInt16(File* SysCsvFile, int16_t* num, char delim);
int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim);
int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim);
int csvReadDouble(File* SysCsvFile, double* num, char delim);
int csvReadFloat(File* SysCsvFile, float* num, char delim);
void csvClose();

// TXT
void dPrint(String s);
void dWrite(unsigned char c);


// Utilities
USING_NAMESPACE_MIDIROUTER
MIDIRouter_Lib router = MIDIRouter_Lib();

// Functions


// Add setup code
void setup()
{
    Serial.begin(115200);
    //  delay(1000);
    if (!SD.begin())
    {
        Serial.println("SD initialization failed!");
        //return;
    } else {
        Serial.println("SD initialization done.");
    }
    
    // knob
    router.SetupEncoder(EncA, EncB, EncSwitch);
    
    // ============================================================
    // EEPROM
    // ============================================================
    loadEEPROM();
    //Serial.print("dacNeg[0] = "); Serial.print(dacNeg[0]); Serial.print(" dacPos[0] = "); Serial.println(dacPos[0]);
    
    // ============================================================
    // DAC 16bit SPI setup
    // ============================================================
    
    // set the CS as an output:
    pinMode (CS, OUTPUT);
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
    //Serial.println("Interface_16x16 Example");
    //delay(10);
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
    
    /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
    while (!tft.begin(RA8875_800x480))  {
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
    //tft.setRotationA(curRot);
    
    // With hardware accelleration this is instant
    tft.graphicsMode();
    tft.fillScreen(RA8875_BLACK);
#ifdef STARTUP_PICTURE
    bmpDraw("WELCOME.BMP", 0, 0);
    delay(1000);
    tft.fillScreen(RA8875_BLACK);
#endif //STARTUP_PICTURE
    /*
     // Play with PWM
     for (uint8_t i = 255; i != 0; i -= 5 ) {
     tft.PWM1out(i);
     delay(10);
     }
     for (uint8_t i = 0; i != 255; i += 5 )  {
     tft.PWM1out(i);
     delay(10);
     }
     tft.PWM1out(255);
     
     //set rotate
     curRot = 2;
     tft.setRotation(curRot);
     
     delay(200);
     tft.fillScreen(RA8875_YELLOW);
     delay(500);
     tft.fillScreen(RA8875_GREEN);
     delay(500);
     tft.fillScreen(RA8875_CYAN);
     delay(500);
     tft.fillScreen(RA8875_MAGENTA);
     delay(500);
     tft.fillScreen(RA8875_BLACK);
     */
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
    if (!SD.chdir("sysexids")) {
        Serial.println("chdir failed for Folder1.\n");
    }
    
    // Open the file.
    SysCsvFile = SD.open("sysexids.csv", FILE_WRITE);
    if (!SysCsvFile) {
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

// Add loop code
void loop()
{
    //routeMidi(); // check incoming MIDi and route it
    if (rdFlag == 1) {
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
