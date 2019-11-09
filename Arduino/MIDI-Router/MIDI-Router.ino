// Touchscreen libraries
#include <GSL1680.h>          // modified firmware, see github.com/insolace
#include <Wire.h>
#include <stdint.h>
#include <SPI.h>
#include "Adafruit_GFX.h"      // vvvvv
#include "Adafruit_RA8875.h"   // both of these are modified, see github.com/insolace

// Memory/storage functions
#include <EEPROM.h>
#include "SdFat.h"
#define USE_SDIO 1
SdFatSdioEX SD;

// Knob
#define EncA 26
#define EncB 27
#define EncSwitch 28
#include <Encoder.h>
Encoder myEnc(EncA, EncB);

#include <Bounce2.h>
Bounce encPush = Bounce(); 

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
#include <MIDI.h>        // access to serial (5 pin DIN) MIDI
#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)

// Create the Serial MIDI ports
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI1);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, MIDI2);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial3, MIDI3);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial4, MIDI4);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial5, MIDI5);
MIDI_CREATE_INSTANCE(HardwareSerial, Serial6, MIDI6);

// Create the ports for USB devices plugged into Teensy's 2nd USB port (via hubs)
USBHost myusb;
USBHub hub1(myusb);
USBHub hub2(myusb);
USBHub hub3(myusb);
USBHub hub4(myusb);
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
int oldPosition = 0;
int knobVal = 0;
int oldKnobVal = 0;
unsigned long knobTimer = millis();
unsigned long knobSlowdown = 4;
int knobSpeedup = 4; // threshold, larger value = less sensitivty to fast turns
int knobSpeedRate = 4;
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
#define   tbColor     0,150,0       // tempo/clock box
#define   hbColor     102,102,102   // setup/home button 
#define   ibColor     0,0,150       // input page box
#define   insColor    0,0,100       // inputs box color
#define   insColFlash 0,0,255       // input flash color
#define   obColor     150,0,0       // output page box
#define   outsColor   100,0,0       // putputs box color
#define   outColFlash 255,0,0       // output flash color
#define   gridColor   102,102,102   // grid
#define   linClr      0, 0, 0       // lines
#define   txColor     255,255,255        // text
#define   routColor   255,255,255   // routing
#define   actFieldBg  0,0,255       // Active Field color
#define   fieldBg     50,50,50      // inactive field color


uint16_t posCol;  // for CV calib
uint16_t negCol;  // for CV calib

uint16_t newColor(uint8_t r, uint8_t g, uint8_t b) 
{
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// end touchscreen defines

#include <elapsedMillis.h>        // for tracking elapsed timers
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
String  inputNames[] = { 
  "Midi1", "Midi2", "Midi3", "Midi4", "Midi5", "Midi6",    // page 0
  "USB1", "USB2", "USB3", "USB4", "USB5", "USB6",         // page 1
  "USB7", "USB8", "USB9", "USB10", "", "",               // page 2
  "Comp1", "Comp2", "Comp3", "Comp4", "Comp5", "Comp6",           // page 3
  "Comp7", "Comp8", "Comp9", "Comp10", "Comp11", "Comp12",           // page 4
  "Comp13", "Comp14", "Comp15", "Comp16", "intClock", "extClock"                 // page 5
}; 

String  outputNames[] = { 
  "Midi1", "Midi2", "Midi3", "Midi4", "Midi5", "Midi6",    // page 0
  "USB1", "USB2", "USB3", "USB4", "USB5", "USB6",         // page 1
  "USB7", "USB8", "USB9", "USB10", "", "",               // page 2
  "Comp1", "Comp2", "Comp3", "Comp4", "Comp5", "Comp6",           // page 3
  "Comp7", "Comp8", "Comp9", "Comp10", "Comp11", "Comp12",           // page 4
  "Comp13", "Comp14", "Comp15", "Comp16", "", "",                 // page 5
  "CV1", "CV2", "CV3", "CV4", "CV5", "CV6"           // page 6
}; 

// Menu options
int menu = 0;  // which menu are we looking at?  0 = routing, 1 = CV calibration
int actField = 1; // which data entry field on the page is active?

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

// Initial routing
// matrix for routing
int routing[50][50] = {  // [input port][output port]
 
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
