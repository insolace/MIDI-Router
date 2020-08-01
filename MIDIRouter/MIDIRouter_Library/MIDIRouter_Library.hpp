/*
  MidiRouter_Lib.hpp - Library for Midi Router Device Functions.
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/
#pragma once

#ifndef MIDIROUTER_LIBRARY_HPP
#define MIDIROUTER_LIBRARY_HPP

BEGIN_MIDIROUTER_NAMESPACE

MRInputPort MIDIRouter_Lib::inputs[] = {
	// page 0
		MRInputPort("DIN 1"),
		MRInputPort("DIN 2"),
		MRInputPort("DIN 3"),
		MRInputPort("DIN 4"),
		MRInputPort("DIN 5"),
		MRInputPort("DIN 6"),
	// page 1
		MRInputPort("USB 1"),
		MRInputPort("USB 2"),
		MRInputPort("USB 3"),
		MRInputPort("USB 4"),
		MRInputPort("USB 5"),
		MRInputPort("USB 6"),
	// page 2
		MRInputPort("USB 7"),
		MRInputPort("USB 8"),
		MRInputPort("USB 9"),
		MRInputPort("USB 10"),
		MRInputPort(""),
		MRInputPort(""),
	// page 3
		MRInputPort("DAW 1"),
		MRInputPort("DAW 2"),
		MRInputPort("DAW 3"),
		MRInputPort("DAW 4"),
		MRInputPort("DAW 5"),
		MRInputPort("DAW 6"),
	// page 4
		MRInputPort("DAW 7"),
		MRInputPort("DAW 8"),
		MRInputPort("DAW 9"),
		MRInputPort("DAW 10"),
		MRInputPort("DAW 11"),
		MRInputPort("DAW 12"),
	// page 5
		MRInputPort("DAW 13"),
		MRInputPort("DAW 14"),
		MRInputPort("DAW 15"),
		MRInputPort("DAW 16"),
		MRInputPort(""),
		MRInputPort("")
};
		
MROutputPort MIDIRouter_Lib::outputs[] = {
		// page 0
		MROutputPort("DIN 1"),
		MROutputPort("DIN 2"),
		MROutputPort("DIN 3"),
		MROutputPort("DIN 4"),
		MROutputPort("DIN 5"),
		MROutputPort("DIN 6"),
	// page 1
		MROutputPort("USB 1"),
		MROutputPort("USB 2"),
		MROutputPort("USB 3"),
		MROutputPort("USB 4"),
		MROutputPort("USB 5"),
		MROutputPort("USB 6"),
	// page 2
		MROutputPort("USB 7"),
		MROutputPort("USB 8"),
		MROutputPort("USB 9"),
		MROutputPort("USB 10"),
		MROutputPort(""),
		MROutputPort(""),
	// page 3
		MROutputPort("DAW 1"),
		MROutputPort("DAW 2"),
		MROutputPort("DAW 3"),
		MROutputPort("DAW 4"),
		MROutputPort("DAW 5"),
		MROutputPort("DAW 6"),
	// page 4
		MROutputPort("DAW 7"),
		MROutputPort("DAW 8"),
		MROutputPort("DAW 9"),
		MROutputPort("DAW 10"),
		MROutputPort("DAW 11"),
		MROutputPort("DAW 12"),
	// page 5
		MROutputPort("DAW 13"),
		MROutputPort("DAW 14"),
		MROutputPort("DAW 15"),
		MROutputPort("DAW 16"),
		MROutputPort(""),
		MROutputPort(""),
	// page 6
        MROutputPort("CV A:D 1"),
        MROutputPort("CV A:D 2"),
        MROutputPort("CV A:D 3"),
        MROutputPort("CV A:D 4"),
        MROutputPort("CV A:D 5"),
        MROutputPort("CV A:D 6")
	};
	
MidiFilter MIDIRouter_Lib::inputPortFilter = MidiFilter();

MRInputPort * MIDIRouter_Lib::inputAt(int index) {
	int size = *(&inputs + 1) - inputs;
	if (index >= size) { return NULL; }
	if (inputs[index].active() == false) { return NULL; }
	return &inputs[index];
};

MROutputPort * MIDIRouter_Lib::outputAt(int index) {
	int size = *(&outputs + 1) - outputs;
	if (index >= size) { return NULL; }
	return &outputs[index];
};

MIDIRouter_Lib::MIDIRouter_Lib() {
}

void MIDIRouter_Lib::SetupEncoder(uint8_t encpin1, uint8_t encpin2, uint8_t enc_button_pin) {
    _encoder = new Encoder(encpin1, encpin2);
    _encPush = new Bounce();
    pinMode(enc_button_pin, INPUT_PULLUP);
    _encPush->attach(enc_button_pin);
    _encPush->interval(100);
}

Encoder &MIDIRouter_Lib::encoder() {
    return *_encoder;
}

Bounce &MIDIRouter_Lib::encPush() {
    return *_encPush;
}

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_LIBRARY_HPP
