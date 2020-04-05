#ifndef _MIDIROUTER_LIB_H
#define _MIDIROUTER_LIB_H

#import "MRPorts.h"
#import "MidiFilter.h"

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