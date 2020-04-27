//
//  MRIOInterface.hpp
//  Index
//
//  Created by Kurt Arnlund on 4/14/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//
#pragma once

#ifndef MIDIROUTER_MRIOINTERFACE_HPP
#define MIDIROUTER_MRIOINTERFACE_HPP

BEGIN_MIDIROUTER_NAMESPACE

// MARK: - MRIOInterface

MRIOInterface::MRIOInterface(unsigned char inPort, unsigned char outPort) {
    input = inPort;
    output = outPort;
}

// MARK: - MRIO_MidiHardwareSerialInterface

MRIO_MidiHardwareSerialInterface::MRIO_MidiHardwareSerialInterface(unsigned char inPort, unsigned char outPort, SerialPort& inSerial) :  MRIOInterface(inPort, outPort) {
    interface = midi::MidiInterface<HardwareSerial>(inSerial)
}

// MARK: - MRIO_MidiUSBClientInterface

MRIO_MidiUSBClientInterface::MRIO_MidiUSBClientInterface(unsigned char inPort, unsigned char outPort) :  MRIOInterface(inPort, outPort) {
}

// MARK: - MRIO_MidiUSBHostInterface

MRIO_MidiUSBHostInterface::MRIO_MidiUSBHostInterface(unsigned char inPort, unsigned char outPort) :  MRIOInterface(inPort, outPort) {
}

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_MRIOINTERFACE_HPP
