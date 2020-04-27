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

class MRIOInterface {
    public:

    unsigned char input;
    unsigned char output;
    
    MRIOInterface(unsigned char inPort, unsigned char outPort);
};

template<class SerialPort>
class MRIO_MidiHardwareSerialInterface : public MRIOInterface {
    public:
    
    midi::MidiInterface<HardwareSerial> interface;
    
    MRIOMidiSerialInterface(unsigned char inPort, unsigned char outPort, SerialPort& inSerial);
};

class MRIO_MidiUSBClientInterface : public MRIOInterface {

    MRIO_MidiUSBClientInterface(unsigned char inPort, unsigned char outPort);
};

class MRIO_MidiUSBHostInterface : public MRIOInterface {

    MRIO_MidiUSBHostInterface(unsigned char inPort, unsigned char outPort);
};


END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_LIBRARY_MRIOInterface_H

#include "MRIOInterface.hpp"

