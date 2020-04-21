/*
  MidiRouter_Lib.h - Library for Midi Router Device Functions.
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/

#include "MIDIRouter_Lib_Defs.h"
#include "MRPorts.h"
#include "MidiFilter.h"

BEGIN_MIDIROUTER_NAMESPACE

struct MIDIRouterSetup {
    uint8_t encpin1;
    uint8_t encpin2;
    uint8_t enc_button_pin;
};

class MIDIRouter_Lib {
	public:
	
	MIDIRouter_Lib();
	
	MRInputPort *inputAt(int index);
	MROutputPort *outputAt(int index);
	static MidiFilter inputPortFilter;
    
    void SetupEncoder(uint8_t encpin1, uint8_t encpin2, uint8_t enc_button_pin);
    Encoder &encoder();
    Bounce &encPush();
	
	private:
	static MRInputPort inputs[];
	static MROutputPort outputs[];
    
    Encoder *_encoder;
    Bounce *_encPush;
};

END_MIDIROUTER_NAMESPACE

#include "MIDIRouter_Lib.hpp"
