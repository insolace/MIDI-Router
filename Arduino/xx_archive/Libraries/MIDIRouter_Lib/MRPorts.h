#ifndef _MR_PORT_H
#define _MR_PORT_H

class MRPort {
	public:
	
  	char name[9];
	unsigned int index;
	MidiFilterLocation location;
	
	MRPort(const char *displayName, unsigned int newindex);
	
	bool active();
};

class MRInputPort : public MRPort {
	public: 
		MRInputPort(const char *displayName);
	
	private:
		static unsigned int inputPortCount;

};

class MROutputPort : public MRPort {
	public: 
		MROutputPort(const char *displayName);

	private:
		static unsigned int outputPortCount;		
};

#endif // _MR_PORT_H