//
//  MIDIRouter_Library_Defs.h
//  MIDIRouter
//
//  Created by Kurt Arnlund on 4/25/20.
//  Copyright © 2020 Isolace / Ingenious Arts and Technologies LLC. All rights reserved.
//

#include "MIDIRouter_Library_Namespace.h"

#pragma once

#ifndef MIDIROUTER_LIBRARY_DEFS_H
#define MIDIROUTER_LIBRARY_DEFS_H

// Core library for code-sense - IDE-based
// !!! Help: https://bit.ly/2AdU7cu
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
#elif defined(ARDUINO) // Arduino 1.0 and 1.5 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE


//#ifndef ARDUINO
//#include <inttypes.h>
//typedef uint8_t byte;
//#endif


BEGIN_MIDIROUTER_NAMESPACE

#ifndef MIDIRouter_RELEASE
///
/// @brief    Release
///
#define MIDIRouter_RELEASE 100
#define MIDIRouter_LIBRARY_VERSION        0x000100
#define MIDIRouter_LIBRARY_VERSION_MAJOR  0
#define MIDIRouter_LIBRARY_VERSION_MINOR  1
#define MIDIRouter_LIBRARY_VERSION_PATCH  0

#endif // MIDIRouter_RELEASE


/*
#define MIDIROUTER_ADD_HARDWARE_IO_INSTANCE(SerialPort, Name)                               \
{                                                                                           \
    midiRouter::MRIO_MidiHardwareSerialInterface<HardwareSerial> Name((Type&)SerialPort);   \
}
*/

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_LIBRARY_DEFS_H
