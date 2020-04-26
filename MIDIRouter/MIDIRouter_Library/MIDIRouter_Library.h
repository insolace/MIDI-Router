///
/// @mainpage	MIDIRouter Library
///
/// @details	MIDI Router based on Teensy 3.6
/// @n			Library
/// @n
/// @n @a		Developed with embedXcode+: https://embedXcode.weebly.com
///
/// @author		Kurt Arnlund
/// @author		Isolace / Ingenious Arts and Technologies LLC
///
/// @date		4/25/20 6:04 PM
/// @version	<#version#>
///
/// @copyright	(c) Kurt Arnlund, 2020
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
///


///
/// @file		MIDIRouter_Library.h
/// @brief		Library header
/// Project 	MIDIRouter Library
///
/// @details	<#details#>
/// @n
/// @n @b		Project MIDIRouter
/// @n @a		Developed with embedXcode+: https://embedXcode.weebly.com
///
/// @author		Kurt Arnlund
/// @author		Isolace / Ingenious Arts and Technologies LLC
///
/// @date		4/25/20 6:04 PM
/// @version	<#version#>
///
/// @copyright	(c) Kurt Arnlund, 2020
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
///
// !!! Help: https://bit.ly/2ifs2e2

#pragma once

//#ifndef MIDIROUTER_LIBRARY_H
//#define MIDIROUTER_LIBRARY_H

#include "MIDIRouter_Library_Defs.h"
#include "MRPorts.h"
#include "MidiFilter.h"
#include <Encoder.h>
#include <Bounce2.h>

BEGIN_MIDIROUTER_NAMESPACE

struct MIDIRouterSetup {
    uint8_t encpin1;
    uint8_t encpin2;
    uint8_t enc_button_pin;
};

class MIDIRouter_Lib {
    public:
    
    MIDIRouter_Lib();
    
    MRInputPort *inputAt(int index);
    MROutputPort *outputAt(int index);
    static MidiFilter inputPortFilter;
    
    void SetupEncoder(uint8_t encpin1, uint8_t encpin2, uint8_t enc_button_pin);
    Encoder &encoder();
    Bounce &encPush();
    
    private:
    static MRInputPort inputs[];
    static MROutputPort outputs[];
    
    Encoder *_encoder;
    Bounce *_encPush;
};

END_MIDIROUTER_NAMESPACE

#include "MIDIRouter_Library.hpp"

//#endif // MIDIROUTER_LIBRARY_H

