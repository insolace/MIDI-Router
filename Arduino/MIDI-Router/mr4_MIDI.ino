void routeMidi() {

  // ============================================================
  // RX Hardware DIN MIDI ports
  // ============================================================
  if (MIDI1.read()) {
    //sendToComputer(MIDI1.getType(), MIDI1.getData1(), MIDI1.getData2(), MIDI1.getChannel(), MIDI1.getSysExArray(), 0);
    // get the USB MIDI message from computer, defined by these 5 numbers (except SysEX)
    byte type = MIDI1.getType();
    byte data1 = MIDI1.getData1();
    byte data2 = MIDI1.getData2();
    byte channel = MIDI1.getChannel();
    const uint8_t *sys = MIDI1.getSysExArray();
    byte port = 0;
    
    if (type != 0) {      
      transmitMIDI(type, data1, data2, channel, port);
    } else {
      transmitSysEx(data1 + data2 * 256, sys, port);     
    }
  }

// brute force copy/paste for now, optimize later
  if (MIDI2.read()) {
    byte type = MIDI2.getType();
    byte data1 = MIDI2.getData1();
    byte data2 = MIDI2.getData2();
    byte channel = MIDI2.getChannel();
    const uint8_t *sys = MIDI2.getSysExArray();
    byte port = 1;
    if (type != 0) {      
      transmitMIDI(type, data1, data2, channel, port);
    } else {
      transmitSysEx(data1 + data2 * 256, sys, port);     
    }
  }

  if (MIDI3.read()) {
    byte type = MIDI3.getType();
    byte data1 = MIDI3.getData1();
    byte data2 = MIDI3.getData2();
    byte channel = MIDI3.getChannel();
    const uint8_t *sys = MIDI3.getSysExArray();
    byte port = 2;
    if (type != 0) {      
      transmitMIDI(type, data1, data2, channel, port);
    } else {
      transmitSysEx(data1 + data2 * 256, sys, port);     
    }
  }

  // ============================================================
  // RX USB DEVICE MIDI ports
  // ============================================================
  
  // Next read messages arriving from the (up to) 10 USB devices plugged into the MIDIROUTER USB devices port
  for (int port=0; port < 10; port++) {
    if (midilist[port]->read()) {
      uint8_t type =       midilist[port]->getType();
      uint8_t data1 =      midilist[port]->getData1();
      uint8_t data2 =      midilist[port]->getData2();
      uint8_t channel =    midilist[port]->getChannel();
      const uint8_t *sys = midilist[port]->getSysExArray();
      if (type != 0) {
/*
        Serial.print("USB ");
        Serial.print("ch=");
        Serial.print(channel, DEC);
        Serial.print(", type=");
        Serial.print(type, DEC);
        Serial.print(", d1=");
        Serial.println(data1, DEC);
        Serial.println((const char *)midi01.manufacturer());
        Serial.println((const char *)midi01.product());
        Serial.println((const char *)midi01.serialNumber());
*/        
        transmitMIDI(type, data1, data2, channel, port + 6);
      } else {
        transmitSysEx(data1 + data2 * 256, sys, port + 6);     
      }
    }
  }
  
  // ============================================================
  // RX USB HOST MIDI ports
  // ============================================================

  // Read USB HOST (computer/DAW) messages the PC sends to Teensy, reroute them based on matrix

  if (usbMIDI.read()) {
    // get the USB MIDI message from computer, defined by these 5 numbers (except SysEX)
    byte type = usbMIDI.getType();
    byte data1 = usbMIDI.getData1();
    byte data2 = usbMIDI.getData2();
    byte channel = usbMIDI.getChannel();
    byte cable = usbMIDI.getCable() + 18; // 18 is offset for USB Host
    if (type != 0) {
      transmitMIDI(type, data1, data2, channel, cable);
    } else {
      transmitSysEx(data1 + data2 * 256, usbMIDI.getSysExArray(), cable);     
    }
  }
}

void transmitMIDI(int t, int d1, int d2, int ch, byte inPort) {

  Serial.print("txMIDI: t");
  Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.print(ch); Serial.print(" inp:"), Serial.println(inPort);

  // Normal messages, first we must convert usbMIDI's type (an ordinary
  // byte) to the MIDI library's special MidiType.
  midi::MidiType mtype = (midi::MidiType)t;
  
  //Serial.print("mtype: "); Serial.println(mtype);
  
  // Route to 6 MIDI DIN Ports
  for (int outp = 0; outp < 6; outp++){
    if (routing[inPort][outp] != 0) {
      switch (outp) {
        case  0: MIDI1.send(mtype, d1, d2, ch); Serial.print("m1: "); Serial.println(t); break;
        case  1: MIDI2.send(mtype, d1, d2, ch); break;
        case  2: MIDI3.send(mtype, d1, d2, ch); break;
        case  3: MIDI4.send(mtype, d1, d2, ch); break;
        case  4: MIDI5.send(mtype, d1, d2, ch); break;
        case  5: MIDI6.send(mtype, d1, d2, ch); break;
        //default:
      }
    }
  }
      
  // Route to 10 USB Devices
  for (int outp = 6; outp < 16; outp++){
    if (routing[inPort][outp] != 0) {
      midilist[outp - 6]->send(t, d1, d2, ch);
    }
  }
 
  // Route to 16 USB Host Ports
  for (int outp = 18; outp < 34; outp++){
    if (routing[inPort][outp] != 0) {
      usbMIDI.send(t, d1, d2, ch, outp - 18);
      //midi::MidiType mtype = (midi::MidiType)t;
      //MIDI1.send(mtype, d1, d2, ch);
      //MIDI2.send(mtype, d1, d2, ch);
      //MIDI3.send(mtype, d1, d2, ch);
    }
  }
}

void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort) {
  Serial.print("txSysex: len:");
  Serial.print(len); Serial.print("array:"), Serial.print("xxx"); Serial.print("inp:"), Serial.print(inPort);

  // Route to USB Host 
  for (int outp = 18; outp < 34; outp++){
    if (routing[inPort][outp] != 0) {
      usbMIDI.sendSysEx(len, sysexarray, true, outp - 18);
    }
  }
}
