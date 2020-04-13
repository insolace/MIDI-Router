/*
  MidiRouter_Lib.h - Library for Midi Router Device Functions.
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/

#ifndef _MIDIROUTER_LIB_H
#define _MIDIROUTER_LIB_H

#include "Arduino.h"
#include "MRPorts.h"
#include "MidiFilter.h"

class MIDIRouter_Lib {
	public:
	
	MIDIRouter_Lib();
	
	MRInputPort *inputAt(int index);
	MROutputPort *outputAt(int index);
	static MidiFilter inputPortFilter;
	
	private:
	static MRInputPort inputs[];
	static MROutputPort outputs[];
};

#endif // _MIDIROUTER_LIB_H