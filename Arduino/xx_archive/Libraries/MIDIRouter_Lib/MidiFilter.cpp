// MidiFilterType

MidiFilterType::MidiFilterType() {
	//settings = {0};
}

void MidiFilterType::set(MidiFilterLocation location, bool state) {
	unsigned char currentByte = settings[location.byte];
	unsigned char bitMask = 0x01 << location.bit;
	if (state == true) {
		settings[location.byte] = currentByte | bitMask;
	} else {
		settings[location.byte] &= ~bitMask;
	}
}

bool MidiFilterType::get(MidiFilterLocation location) {
	unsigned char currentByte = settings[location.byte];
	unsigned char bitMask = 0x01 << location.bit;
	return (settings[location.byte] & bitMask) > 0 ? true : false;
}

// MidiFilter

MidiFilterType MidiFilter::realtime = MidiFilterType();
MidiFilterType MidiFilter::notes = MidiFilterType();
MidiFilterType MidiFilter::controllers = MidiFilterType();
MidiFilterType MidiFilter::sysex = MidiFilterType();
MidiFilterType MidiFilter::midiclock = MidiFilterType();
MidiFilter::MidiFilter() {
}

void MidiFilter::setRealtime(MidiFilterLocation location, bool state) {
	realtime.set(location, state);
}

void MidiFilter::setNotes(MidiFilterLocation location, bool state) {
	notes.set(location, state);
}

void MidiFilter::setControllers(MidiFilterLocation location, bool state) {
	controllers.set(location, state);
}

void MidiFilter::setSysex(MidiFilterLocation location, bool state) {
	sysex.set(location, state);
}

void MidiFilter::setMidiclock(MidiFilterLocation location, bool state) {
	midiclock.set(location, state);
}

bool MidiFilter::realtimeFiltered(MidiFilterLocation location) {
	return realtime.get(location);
}

bool MidiFilter::notesFiltered(MidiFilterLocation location) {
	return notes.get(location);
}

bool MidiFilter::controllersFiltered(MidiFilterLocation location) {
	return controllers.get(location);
}

bool MidiFilter::sysexFiltered(MidiFilterLocation location) {
	return sysex.get(location);
}

bool  MidiFilter::midiclockFiltered(MidiFilterLocation location) {
	return midiclock.get(location);
}
