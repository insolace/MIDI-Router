//
//  MIDIRouter_Lib_Defs.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/14/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#pragma once

#include "MIDIRouter_Lib_Namespace.h"

//// Core library for code-sense - MCU-based
//#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega2560__) || defined(__AVR_ATmega32U4__) || defined(__SAM3X8E__) || defined(__AVR_ATmega168__) // Arduino specific
//#include "Arduino.h"
//#elif defined(i586) // Galileo specific
//#include "Arduino.h"
//#elif defined(__32MX320F128H__) || defined(__32MX795F512L__) || defined(__32MX340F512H__) // chipKIT specific
//#include "WProgram.h"
//#elif defined(__AVR_ATtinyX5__) // Digispark specific
//#include "Arduino.h"
//#elif defined(__AVR_ATmega644P__) // Wiring specific
//#include "Wiring.h"
//#elif defined(__MSP430G2452__) || defined(__MSP430G2553__) || defined(__MSP430G2231__) || defined(__MSP430F5529__) || defined(__MSP430FR5739__) || defined(__MSP430F5969__) // LaunchPad MSP430 specific
//#include "Energia.h"
//#elif defined(__LM4F120H5QR__) || defined(__TM4C123GH6PM__) || defined(__TM4C129XNCZAD__) || defined(__CC3200R1M1RGC__) // LaunchPad LM4F TM4C CC3200 specific
//#include "Energia.h"
//#elif defined(__MK20DX128__) || defined(__MK20DX256__) // Teensy 3.0 3.1 specific
//#include "WProgram.h"
//#elif defined(__RFduino__) // RFduino specific
//#include "Arduino.h"
//#elif defined(MCU_STM32F103RB) || defined(MCU_STM32F103ZE) || defined(MCU_STM32F103CB) || defined(MCU_STM32F103RE) // Maple specific
//#include "WProgram.h"
//#elif defined(__MK66FX1MO__) || defined(TEENSYDUINO)
//#include "Arduino.h"
//#else // error
//#error Platform not defined or not supported
//#endif

#if ARDUINO
#include <Arduino.h>
#else
#include <inttypes.h>
typedef uint8_t byte;
#endif

#include <Encoder.h>
#include <Bounce2.h>


BEGIN_MIDIROUTER_NAMESPACE

#define MIDIROUTER_LIBRARY_VERSION        0x000100
#define MIDIROUTER_LIBRARY_VERSION_MAJOR  0
#define MIDIROUTER_LIBRARY_VERSION_MINOR  1
#define MIDIROUTER_LIBRARY_VERSION_PATCH  0

/*
#define MIDIROUTER_ADD_HARDWARE_IO_INSTANCE(SerialPort, Name)                               \
{                                                                                           \
    midiRouter::MRIO_MidiHardwareSerialInterface<HardwareSerial> Name((Type&)SerialPort);   \
}
*/

END_MIDIROUTER_NAMESPACE
