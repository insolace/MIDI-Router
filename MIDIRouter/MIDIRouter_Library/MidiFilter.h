/*
  MidiFilter.h - 
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/
#pragma once

#include "MIDIRouter_Library_Defs.h"

#ifndef MIDIROUTER_MIDIFILTER_H
#define MIDIROUTER_MIDIFILTER_H

BEGIN_MIDIROUTER_NAMESPACE

struct ByteBitLocation {
	unsigned int byte;
	unsigned int bit;
};

class MidiFilterType {
	public:
		unsigned char settings[8];
		
		MidiFilterType();
		
	void set(ByteBitLocation location, bool state);
	bool get(ByteBitLocation location);
};

class MidiFilter {
	public:
	MidiFilter();
	
	void setRealtime(ByteBitLocation location, bool state);
	void setNotes(ByteBitLocation location, bool state);
	void setControllers(ByteBitLocation location, bool state);
	void setSysex(ByteBitLocation location, bool state);
	void setMidiclock(ByteBitLocation location, bool state);
	
	bool realtimeFiltered(ByteBitLocation location);
	bool notesFiltered(ByteBitLocation location);
	bool controllersFiltered(ByteBitLocation location);
	bool sysexFiltered(ByteBitLocation location);
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

#include "MidiFilter.hpp"
