/*
 
 MIDI Router
 Created by Eric Bateman (eric at timeline85 dot com)
 http://www.midirouter.com
 
 MR_DRAW.h - MIDI Router graphics
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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "RA8875.h"             // display
#include "GSL1680.h"            // capacitive touch interface, modified firmware, see github.com/insolace
#include <Encoder.h>            // knob
#include <Bounce2.h>            // knob pushbutton
#include "main.hpp"

#ifdef SDCARD_SUPPORT
#include <SD.h>
#include <SPI.h>
#endif

// TFT Display pins
#define RA8875_INT 15 ///< graphic interrupt
#define RA8875_CS 14 ///<  chip select
#define RA8875_RESET 35 ///< reset


// Touch Pins
#define WAKE 16 ///< wakeup! (is this used?)
#define INTRPT 17 ///< touch interrupt

// Knob
#define EncA 26         ///< Encoder pin 1 input
#define EncB 27         ///< Encoder pin 2 input
#define EncSwitch 28    ///< Encoder push button pin input



extern RA8875 tft;
extern RA8875tsize fSize;

extern GSL1680 TS;

extern float clearRouting; 
extern float pi;

extern boolean rdFlag; ///< flag to redraw screen;

void initDisplay();

// CalcColor
/// Color Calculation
/// @param r red 0-255
/// @param g green 0-255
/// @param b blue 0-255
/// @return uint16_t 16 bit color
uint16_t newColor(uint8_t r, uint8_t g, uint8_t b);

// DRAW
void drawBox(); ///< Draw the four boxes in the upper left of the display
void drawColumns(); ///< Draw columns
void drawRows(); ///< Draw rows
void drawRouting(); ///< Draw the current routing grid

/// Draw the current routing point
/// @param c  Column
/// @param r Row
void drawRoute(int c, int r);

/// Draw the current routing point (MIDI)
/// @param c  Column
/// @param r Row
bool drawRouteMidi(int c, int r);

/// Draw the current routing point (CH)
/// @param c  Column
/// @param r Row
void drawRouteCH(int c, int r);

/// Draw the current routing point (CV)
/// @param c  Column
/// @param r Row
bool drawRouteCV(int c, int r);

/// Highlight the current routing point/selection
/// @param c  Column
/// @param r Row
void highlightSelect(int c, int r, bool s);

/// Clear highlight for routing point/selection
/// @param c  Column
/// @param r Row
/// @param s State (on/off)
void blankSelect(int c, int r); ///< Blank the last selection

void drawGLines(); ///< Draw the routing grid lines

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

#ifdef SDCARD_SUPPORT
/// Read a 16 bit value from the current open file
/// @param f File location
/// @return uint16_t 16 bit value
uint16_t read16(File &f);

/// Read a 32 bit value from the current open file
/// @param f File location
/// @return uint32_t 32 bit value
uint32_t read32(File &f);
#endif

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
/// Draw Touchscreen Keyboard
/// @param x X coordinate to draw the keyboard at
/// @param y Y coordinate to draw the keyboard at
void drawKeyboard(int x, int y);



// TXT
/// Print string to display
/// @param s String to print
/// @param sz Font size
void dPrint(String s, int sz = fSize);
/// Print character to display
/// @param c Character
/// @param s Font size
void dWrite(unsigned char c, unsigned int s);
/// Print vertical text (deprecated)
/// @param s String to print
void printVert(String s);
/// Change the font size and store previous size
/// @param ts Font size X16, X24, X32
void fontSize(enum RA8875tsize ts);
/// Print centered text (for ROM fonts that aren't accurate in RA8875.cpp)
/// @param s String to print
/// @param x X coordinate
/// @param y Y coordinate
/// @param ts Font size to print at (if blank then default to current font size). Restores the previous font size after print.
void printCenter(String s, int x, int y, enum RA8875tsize ts = (RA8875tsize)fSize);
/// Get the pixel width of a string printed with ARIEL X24 font ROM
/// @param s String to measure
int getWidthAX24(String s, int * wArray);




// TOUCH
void touchIO(); ///< perform touch i/o
void drawTouchPos(); ///< Display current touch position on the screen
void evaltouch(); ///< Evaluate current touch coordinates
void update_Routing(); ///< Update the routing menu/page
void refMenu_Routing(); ///< Refresh routing menu/page
void refMenu_Calibrate(); ///< Refresh calibration menu/page
void update_Calibrate(); ///< Update the calibration menu
void update_Cal_Values(); ///< Update CV calibration values
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



/// Re-order the filtering options so that the most convinient filters come up first
/// @param r Value to be re-ordered
/// @return int value
int reOrderR(int r);
/// Print font character widths to serial port
void printCharWidths();

// ****************************************************************************************************
// ***************** CLASSES **************************************************************************
// ****************************************************************************************************

// Class that describes a GUI element
class guiElem
{
public:
    /// x/y origin and width/height of element on layer 2
    uint16_t x, y, w, h;

    guiElem (uint16_t, uint16_t, uint16_t, uint16_t);
    void draw(uint16_t destX, uint16_t destY, bool transp);
};




#endif/* GRAPHICS_H */
