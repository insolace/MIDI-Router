/*
 
 MIDI Router
 Created by Eric Bateman (eric at timeline85 dot com)
 http://www.midirouter.com
 
 MR_MIDI.h - MIDI Router MIDI processing
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

#ifndef MIDI_HPP
#define MIDI_HPP

#include <USBHost_t36.h> // access to USB MIDI devices (plugged into 2nd USB port)
#include "serialMIDI.h"
#include "MIDI.h"

extern uint8_t routing[50][50];


enum cvFilt 
{ 
    cvF_UNR = 0, 
    cvF_NOT, 
    cvF_VEL, 
    cvF_PW, 
    cvF_MOD, 
    cvF_CCs, 
    cvF_CC74, 
    cvF_AT
};



// MIDI

void initMidi();

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

/// Set DAC/CV to a calibrated 14bit value (0-16384))
/// @param data Data to send (0-16384))
/// @param dac DAC output to set
/// @return float value
float CV14bitCal(int16_t data, int dac);

/// Print ADC values to Serial port
void showADC();

/// Profile instruments connected to DIN ports using SysEx ID Request
void profileInstruments();

/// Check for added/removed USB devices
void updateUSB();

/// Evaluate MIDI type against filter
/// @param t MIDI Type (noteon, cc, etc)
/// @param ch Channel
/// @param f Filter value
/// @return bool true/false
bool filtRoute(int t, int ch, int f);



#endif/* MIDI_HPP */


