/*
    MidiFilter.h -
    Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
    Released into the public domain.
*/
#ifndef MIDIROUTER_MIDIFILTER_H
#define MIDIROUTER_MIDIFILTER_H

#include "MIDIRouter_Library_Defs.h"



BEGIN_MIDIROUTER_NAMESPACE

/// Structure that describes a specific byte and bit location
struct ByteBitLocation
{
    /// byte location
    unsigned int byte;
    /// bit location
    unsigned int bit;
};

/// MIDI filter object
/// @brief this object is intended to wrap bit flags for a single bit-based feature flag
class MidiFilterType
{
  public:
    MidiFilterType();

    /// Set feature flag to a state
    /// @param location  location of feature flag
    /// @param state  boolean state to set it to
    void set(ByteBitLocation location, bool state);
    /// Get feature flag state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool get(ByteBitLocation location);

  private:
    unsigned char settings[8];
};

/// MIDI Filter object
/// @brief This object contains common midi message filter types and methods to set and get them on port locations
class MidiFilter
{
  public:
    MidiFilter();

    /// Set system realtime filter state
    /// @param location  port location
    /// @param state  feature state
    void setRealtime(ByteBitLocation location, bool state);
    /// Set note filter state
    /// @param location  port location
    /// @param state  feature state
    void setNotes(ByteBitLocation location, bool state);
    /// Set controllers filter state
    /// @param location  port location
    /// @param state  feature state
    void setControllers(ByteBitLocation location, bool state);
    /// Set sysex filter state
    /// @param location  port location
    /// @param state  feature state
    void setSysex(ByteBitLocation location, bool state);
    /// Set midi clock filter state
    /// @param location  port location
    /// @param state  feature state
    void setMidiclock(ByteBitLocation location, bool state);

    /// Get system realtime filter state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool realtimeFiltered(ByteBitLocation location);
    /// Get notes filter state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool notesFiltered(ByteBitLocation location);
    /// Get controllers filter state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool controllersFiltered(ByteBitLocation location);
    /// Get sysex filter state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool sysexFiltered(ByteBitLocation location);
    /// Get midi clock filter state
    /// @param location location of feature flag
    /// @return boolean true/false status, true = filtered
    bool midiclockFiltered(ByteBitLocation location);

  private:
    static MidiFilterType realtime;
    static MidiFilterType notes;
    static MidiFilterType controllers;
    static MidiFilterType sysex;
    static MidiFilterType midiclock;
};

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_MIDIFILTER_H

