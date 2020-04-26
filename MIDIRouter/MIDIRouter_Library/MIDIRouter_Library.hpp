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
		MRInputPort("Midi1"),
		MRInputPort("Midi2"),
		MRInputPort("Midi3"),
		MRInputPort("Midi4"),
		MRInputPort("Midi5"),
		MRInputPort("Midi6"),
	// page 1
		MRInputPort("USB1"),
		MRInputPort("USB2"),
		MRInputPort("USB3"),
		MRInputPort("USB4"),
		MRInputPort("USB5"),
		MRInputPort("USB6"),
	// page 2
		MRInputPort("USB7"),
		MRInputPort("USB8"),
		MRInputPort("USB9"),
		MRInputPort("USB10"),
		MRInputPort(""),
		MRInputPort(""),
	// page 3
		MRInputPort("Comp1"),
		MRInputPort("Comp2"),
		MRInputPort("Comp3"),
		MRInputPort("Comp4"),
		MRInputPort("Comp5"),
		MRInputPort("Comp6"),
	// page 4
		MRInputPort("Comp7"),
		MRInputPort("Comp8"),
		MRInputPort("Comp9"),
		MRInputPort("Comp10"),
		MRInputPort("Comp11"),
		MRInputPort("Comp12"),
	// page 5
		MRInputPort("Comp13"),
		MRInputPort("Comp14"),
		MRInputPort("Comp15"),
		MRInputPort("Comp16"),
		MRInputPort("intClock"),
		MRInputPort("extClock")
};
		
MROutputPort MIDIRouter_Lib::outputs[] = {
		// page 0
		MROutputPort("Midi1"),
		MROutputPort("Midi2"),
		MROutputPort("Midi3"),
		MROutputPort("Midi4"),
		MROutputPort("Midi5"),
		MROutputPort("Midi6"),
	// page 1
		MROutputPort("USB1"),
		MROutputPort("USB2"),
		MROutputPort("USB3"),
		MROutputPort("USB4"),
		MROutputPort("USB5"),
		MROutputPort("USB6"),
	// page 2
		MROutputPort("USB7"),
		MROutputPort("USB8"),
		MROutputPort("USB9"),
		MROutputPort("USB10"),
		MROutputPort(""),
		MROutputPort(""),
	// page 3
		MROutputPort("Comp1"),
		MROutputPort("Comp2"),
		MROutputPort("Comp3"),
		MROutputPort("Comp4"),
		MROutputPort("Comp5"),
		MROutputPort("Comp6"),
	// page 4
		MROutputPort("Comp7"),
		MROutputPort("Comp8"),
		MROutputPort("Comp9"),
		MROutputPort("Comp10"),
		MROutputPort("Comp11"),
		MROutputPort("Comp12"),
	// page 5
		MROutputPort("Comp13"),
		MROutputPort("Comp14"),
		MROutputPort("Comp15"),
		MROutputPort("Comp16"),
		MROutputPort(""),
		MROutputPort(""),
	// page 6
		MROutputPort("CV1"),
		MROutputPort("CV2"),
		MROutputPort("CV3"),
		MROutputPort("CV4"),
		MROutputPort("CV5"),
		MROutputPort("CV6")
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
