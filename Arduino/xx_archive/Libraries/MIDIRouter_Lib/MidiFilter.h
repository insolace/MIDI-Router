#ifndef _MIDI_FILTER_H
#define _MIDI_FILTER_H

struct MidiFilterLocation {
	unsigned int byte;
	unsigned int bit;
};

class MidiFilterType {
	public:
		unsigned char settings[8];
		
		MidiFilterType();
		
	void set(MidiFilterLocation location, bool state);
	bool get(MidiFilterLocation location);
};

class MidiFilter {
	public:
	static MidiFilterType realtime;
	static MidiFilterType notes;
	static MidiFilterType controllers;
	static MidiFilterType sysex;
	static MidiFilterType midiclock;
	
	MidiFilter();
	
	void setRealtime(MidiFilterLocation location, bool state);
	void setNotes(MidiFilterLocation location, bool state);
	void setControllers(MidiFilterLocation location, bool state);
	void setSysex(MidiFilterLocation location, bool state);
	void setMidiclock(MidiFilterLocation location, bool state);
	
	bool realtimeFiltered(MidiFilterLocation location);
	bool notesFiltered(MidiFilterLocation location);
	bool controllersFiltered(MidiFilterLocation location);
	bool sysexFiltered(MidiFilterLocation location);
	bool midiclockFiltered(MidiFilterLocation location);
};

#endif // _MIDI_FILTER_H