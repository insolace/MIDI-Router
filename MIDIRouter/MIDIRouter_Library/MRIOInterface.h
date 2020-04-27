//
//  MRIOInterface.h
//  Index
//
//  Created by Kurt Arnlund on 4/14/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//
#include "MIDIRouter_Library_Defs.h"
#include <MIDI.h>

#pragma once

#ifndef MIDIROUTER_LIBRARY_MRIOInterface_H
#define MIDIROUTER_LIBRARY_MRIOInterface_H

BEGIN_MIDIROUTER_NAMESPACE

/// MIDI Router Input/Output Interface Base Class
/// @brief This object ties and input and an ouput together
class MRIOInterface {
    public:

    /// Index if the input port
    unsigned char input;
    /// Index if the output port
    unsigned char output;
    
    /// Constructor
    /// @param inPort index of the input port
    /// @param outPort index of the output port
    MRIOInterface(unsigned char inPort, unsigned char outPort);
};

template<class SerialPort>
/// MIDI Router Input/Output Interface For Serial Connected MIDI Ports
class MRIO_MidiHardwareSerialInterface : public MRIOInterface {
    public:
    
    /// The Hardware Serial MIDI Interface
    midi::MidiInterface<HardwareSerial> interface;
    
    /// Constructor
    /// @param inPort input port index
    /// @param outPort output port index
    /// @param inSerial serial port to create an interface for
    MRIO_MidiHardwareSerialInterface(unsigned char inPort, unsigned char outPort, SerialPort& inSerial);
};

/// DescriptionMIDI Router Input/Output Interface For USB Connected MIDI Ports
class MRIO_MidiUSBClientInterface : public MRIOInterface {

    MRIO_MidiUSBClientInterface(unsigned char inPort, unsigned char outPort);
};

/// DescriptionMIDI Router Input/Output Interface For USB Connected Host Interface MIDI Ports
class MRIO_MidiUSBHostInterface : public MRIOInterface {

    MRIO_MidiUSBHostInterface(unsigned char inPort, unsigned char outPort);
};


END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_LIBRARY_MRIOInterface_H

#include "MRIOInterface.hpp"

