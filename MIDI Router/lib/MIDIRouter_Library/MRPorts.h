/*
    MRPorts.h - Midi Router Port Wrapper
    Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
    Released into the public domain.
*/

#ifndef MIDIROUTER_MIDIPORTS_H
#define MIDIROUTER_MIDIPORTS_H

#include "MIDIRouter_Library_Defs.h"
#include "MidiFilter.h"

#ifndef PortNameCharacterCountPlusNullTerm
#define PortNameCharacterCountPlusNullTerm  9
#endif

BEGIN_MIDIROUTER_NAMESPACE

/// MIDI Router Port Base Class
class MRPort
{
  public:

    /// port name storage
    char name[PortNameCharacterCountPlusNullTerm];
    /// port index
    unsigned int index;
    /// byte and bit location for this port
    /// @brief This can be used to access port feature flag bits
    ByteBitLocation location;

    /// Port Constructor
    /// @details [KURT] add details explainging the newIndex
    /// @param displayName the name to display for the port
    /// @param newindex  index of the port
    MRPort(const char *displayName, unsigned int newindex);

    /// Is the port active
    /// @return boolean true/false is the port active.  Inactive ports as used as filler.
    bool active();
};

/// MIDI Router Input Port
class MRInputPort : public MRPort
{
  public:
    /// Constructor
    /// @param displayName the name to display for the port
    MRInputPort(const char *displayName);

  private:
    static unsigned int inputPortCount;

};

/// DescriptionMIDI Router Output Port
class MROutputPort : public MRPort
{
  public:
    /// Constructor
    /// @param displayName the name to display for the port
    MROutputPort(const char *displayName);

  private:
    static unsigned int outputPortCount;
};

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_MIDIPORTS_HPP
