/*
  MidiFilter.cpp - 
  Created by Kurt R. Arnlund (Ingenious Arts & Technologies LLC), April 4, 2020.
  Released into the public domain.
*/
#pragma once

#ifndef MIDIROUTER_MIDIFILTER_HPP
#define MIDIROUTER_MIDIFILTER_HPP

BEGIN_MIDIROUTER_NAMESPACE

// MidiFilterType

MidiFilterType::MidiFilterType() {
}

void MidiFilterType::set(ByteBitLocation location, bool state) {
	unsigned char currentByte = settings[location.byte];
	unsigned char bitMask = 0x01 << location.bit;
	if (state == true) {
		settings[location.byte] = currentByte | bitMask;
	} else {
		settings[location.byte] &= ~bitMask;
	}
}

bool MidiFilterType::get(ByteBitLocation location) {
	unsigned char currentByte = settings[location.byte];
	unsigned char bitMask = 0x01 << location.bit;
	return (currentByte & bitMask) > 0 ? true : false;
}

// MidiFilter

MidiFilterType MidiFilter::realtime = MidiFilterType();
MidiFilterType MidiFilter::notes = MidiFilterType();
MidiFilterType MidiFilter::controllers = MidiFilterType();
MidiFilterType MidiFilter::sysex = MidiFilterType();
MidiFilterType MidiFilter::midiclock = MidiFilterType();
MidiFilter::MidiFilter() {
}

void MidiFilter::setRealtime(ByteBitLocation location, bool state) {
	realtime.set(location, state);
}

void MidiFilter::setNotes(ByteBitLocation location, bool state) {
	notes.set(location, state);
}

void MidiFilter::setControllers(ByteBitLocation location, bool state) {
	controllers.set(location, state);
}

void MidiFilter::setSysex(ByteBitLocation location, bool state) {
	sysex.set(location, state);
}

void MidiFilter::setMidiclock(ByteBitLocation location, bool state) {
	midiclock.set(location, state);
}

bool MidiFilter::realtimeFiltered(ByteBitLocation location) {
	return realtime.get(location);
}

bool MidiFilter::notesFiltered(ByteBitLocation location) {
	return notes.get(location);
}

bool MidiFilter::controllersFiltered(ByteBitLocation location) {
	return controllers.get(location);
}

bool MidiFilter::sysexFiltered(ByteBitLocation location) {
	return sysex.get(location);
}

bool  MidiFilter::midiclockFiltered(ByteBitLocation location) {
	return midiclock.get(location);
}

END_MIDIROUTER_NAMESPACE

#endif // MIDIROUTER_MIDIFILTER_HPP
