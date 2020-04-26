/*
  MidiPorts.h - Midi Router Port Wrapper
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/
#pragma once

#ifndef MIDIROUTER_MIDIPORTS_HPP
#define MIDIROUTER_MIDIPORTS_HPP

BEGIN_MIDIROUTER_NAMESPACE

MRPort::MRPort(const char *displayName, unsigned int newindex) {
    strcpy(name, displayName);
    index = newindex;
    location.byte = index / 8;
    location.bit = index % 8;
}

bool MRPort::active() {
    if (name[0] == 0) { return false; }
    return true;
}

// MARK: - MRInputPort

unsigned int MRInputPort::inputPortCount = 0;

MRInputPort::MRInputPort(const char *displayName) : MRPort(displayName, inputPortCount) {
    if (active()) {
        inputPortCount += 1;
    }
}

// MARK: - MROutputPort

unsigned int MROutputPort::outputPortCount = 0;

MROutputPort::MROutputPort(const char *displayName) : MRPort(displayName, outputPortCount) {
    if (active()) {
        outputPortCount += 1;
    }
}

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_MIDIPORTS_HPP
