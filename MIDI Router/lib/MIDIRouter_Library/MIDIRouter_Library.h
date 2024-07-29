///
/// @file		MIDIRouter_Library.h
/// @brief		Library header
/// Project 	MIDIRouter Library
///
/// @details	MIDI Router Library
/// @n
/// @n @b		Project MIDIRouter
/// @n @a		Developed with embedXcode+: https://embedXcode.weebly.com
///
/// @author		Kurt Arnlund
/// @author		Timeline85 / Ingenious Arts and Technologies LLC
///
/// @date		4/25/20 6:04 PM
/// @version	0.0.1
///
/// @copyright	(c) Kurt Arnlund, 2020
/// @copyright	GNU General Public Licence
///
/// @see		ReadMe.txt for references
///
// !!! Help: https://bit.ly/2ifs2e2

#ifndef MIDIROUTER_LIBRARY_H
#define MIDIROUTER_LIBRARY_H

#include "MIDIRouter_Library_Defs.h"
#include "MRPorts.h"
#include "MidiFilter.h"
#include <Encoder.h>
#include <Bounce2.h>

BEGIN_MIDIROUTER_NAMESPACE

/// Structure that describes the pins used for a midi router setup
struct MIDIRouterSetup
{
    /// Encoder pin 1
    uint8_t encpin1;
    /// Encoder pin 2
    uint8_t encpin2;
    /// Encoder button pin
    uint8_t enc_button_pin;
};

/// MIDI Router Library object
class MIDIRouter_Lib
{
  public:

    MIDIRouter_Lib();

    /// Locate a midi input port
    /// @param index index of port
    /// @return pointer to MRInputPort object
    MRInputPort *inputAt(int index);
    /// Locate a midi output port
    /// @param index index of port
    /// @return pointer to MROutputPort object
    MROutputPort *outputAt(int index);
    /// Input port filter object
    static MidiFilter inputPortFilter;

    /// Setup an encoder
    /// @brief [KURT] this may possibly be changed in the future to us a MIDIRouterSetup structure for input
    /// @param encpin1 encoder pin 1 input
    /// @param encpin2  encoder pin 2 input
    /// @param enc_button_pin  encoder push button pin input
    void SetupEncoder(uint8_t encpin1, uint8_t encpin2, uint8_t enc_button_pin);
    /// Encoder object
    /// @return Encoder object reference
    Encoder &encoder();
    /// Encoder push button object
    /// @return Bounce button debouncing object reference
    Bounce &encPush();

  private:
    static MRInputPort inputs[];
    static MROutputPort outputs[];

    Encoder *_encoder;
    Bounce *_encPush;
};

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_LIBRARY_H

