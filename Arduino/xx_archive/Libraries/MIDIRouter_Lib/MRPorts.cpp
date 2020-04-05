MRPort::MRPort(const char *displayName, unsigned int newindex) {
	strcpy(name, displayName);
	index = newindex;
	location.byte = index / 8;
	location.bit = index % 8;
}

bool MRPort::active() {
	if (name[0] == 0) { return false; }
	return true;
}

unsigned int MRInputPort::inputPortCount = 0;

MRInputPort::MRInputPort(const char *displayName) : MRPort(displayName, inputPortCount) {
	if (active()) {
		inputPortCount += 1;
	}
}

unsigned int MROutputPort::outputPortCount = 0;

MROutputPort::MROutputPort(const char *displayName) : MRPort(displayName, outputPortCount) {
	if (active()) {
		outputPortCount += 1;
	}
}
