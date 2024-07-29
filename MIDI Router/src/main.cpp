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

#include "main.hpp"

#include "touchscreen.hpp"
#include "midi.hpp"
#include "storage.hpp"
#include "cvDAC.hpp"
#include <elapsedMillis.h>

// ============================================================
// variables/objects begin here
// ============================================================

IntervalTimer callMIDI; ///< Interrupt driven Interval timer for midi io scheduling
#define INTERVALMIDI 250 ///< how often (in microseconds) we call routeMidi()


USING_NAMESPACE_MIDIROUTER
MIDIRouter_Lib router = MIDIRouter_Lib(); ///< Create MIDI Router Object

// Functions

///! Add setup code
void setup()
{
    Serial.begin(115200);

#ifdef SDCARD_SUPPORT
    initSD();
#endif // SDCARD_SUPPORT

    router.SetupEncoder(EncA, EncB, EncSwitch);
    loadEEPROM();
    initMidi();

    // ============================================================
    // Display and Touchscreen setup
    // ============================================================
#ifdef TFT_DISPLAY
    initDisplay();
#endif // TFT_DISPLAY
    
    // sysex id req
    //profileInstruments();
    
#ifdef TFT_DISPLAY
    
#ifdef STARTUP_PICTURE
    
    bmpDraw("WELCOME.BMP", 0, 0);

#endif //STARTUP_PICTURE

    tft.backlight(1);     // backlight on
    delay(2000);
    
    // Setup Touch
    TS.begin(WAKE, INTRPT);       // touch firmware takes approx 2.8s to load
    tft.clearScreen(RA8875_BLACK);
    
    tft.layerEffect(LAYER1); // layer 1, 8bit color
    tft.writeTo(L2);
    bmpDraw("Skins/default/skin_default.bmp", 0, 0);
    tft.writeTo(L1); // layer 1
    
    randomSeed(analogRead(0));
    
    /* below keyboard code is still under development
    kb_shift = kCaps;
    drawKeyboard(kb_x, kb_y);

    delay(1000);
    
    kb_shift = kLower;
    drawKeyboard(kb_x, kb_y);
    
    delay(1000);
    
    kb_shift = kSym;
    drawKeyboard(kb_x, kb_y);
    
    delay(1000);
    

    tft.setFontSize(X32);
    printCharWidths();
    tft.setFontSize(X24);
    Serial.println("\n end widths");
    delay(5000);
     */
    
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



#ifdef TFT_DISPLAY

    // draw homescreen
    refMenu_Routing();
    rdFlag = false;
#endif // TFT_DISPLAY

    initDAC();

}

///! Add loop code
void loop()
{
    if (rdFlag == true)
    {
        rdFlag = false;
        refMenu_Routing();
    }
    readKnob();  // check knob for turn/push
    touchIO();   // process touchscreen input
    updateUSB(); // check for new/removed USB devices

    //**************************

    // test for active input flashes, turn off after interval (still in development)
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


