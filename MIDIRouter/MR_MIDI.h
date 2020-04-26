//
//  MR_MIDI.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef MR_MIDI_h
#define MR_MIDI_h

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
        
        if (type != 240) {
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
        if (type != 240) {
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
        if (type != 240) {
            transmitMIDI(type, data1, data2, channel, port);
        } else {
            transmitSysEx(data1 + data2 * 256, sys, port);
        }
    }
    
    if (MIDI4.read()) {
        byte type = MIDI4.getType();
        byte data1 = MIDI4.getData1();
        byte data2 = MIDI4.getData2();
        byte channel = MIDI4.getChannel();
        const uint8_t *sys = MIDI4.getSysExArray();
        byte port = 3;
        if (type != 240) {
            transmitMIDI(type, data1, data2, channel, port);
        } else {
            transmitSysEx(data1 + data2 * 256, sys, port);
        }
    }
    
    if (MIDI5.read()) {
        byte type = MIDI5.getType();
        byte data1 = MIDI5.getData1();
        byte data2 = MIDI5.getData2();
        byte channel = MIDI5.getChannel();
        const uint8_t *sys = MIDI5.getSysExArray();
        byte port = 4;
        if (type != 240) {
            transmitMIDI(type, data1, data2, channel, port);
        } else {
            transmitSysEx(data1 + data2 * 256, sys, port);
        }
    }
    
    if (MIDI6.read()) {
        byte type = MIDI6.getType();
        byte data1 = MIDI6.getData1();
        byte data2 = MIDI6.getData2();
        byte channel = MIDI6.getChannel();
        const uint8_t *sys = MIDI6.getSysExArray();
        byte port = 5;
        if (type != 240) {
            transmitMIDI(type, data1, data2, channel, port);
        } else {
            transmitSysEx(data1 + data2 * 256, sys, port);
        }
    }
    // ============================================================
    // RX USB DEVICE MIDI ports
    // ============================================================
    
    // Next read messages arriving from the (up to) 10 USB devices plugged into the MIDIROUTER USB devices port
    for (int port = 0; port < 10; port++) {
        if (midilist[port]->read()) {
            uint8_t type =       midilist[port]->getType();
            uint8_t data1 =      midilist[port]->getData1();
            uint8_t data2 =      midilist[port]->getData2();
            uint8_t channel =    midilist[port]->getChannel();
            const uint8_t *sys = midilist[port]->getSysExArray();
            if (type != 240) {
                /*
                 Serial.print("USB ");
                 Serial.print("ch=");
                 Serial.print(channel, DEC);
                 Serial.print(", type=");
                 Serial.print(type, DEC);
                 Serial.print(", d1=");
                 Serial.println(data1, DEC);
                 Serial.print("MFG: "); Serial.println((const char *)midi01.manufacturer());
                 Serial.print("Prod: "); Serial.println((const char *)midi01.product());
                 Serial.print("Serial: "); Serial.println((const char *)midi01.serialNumber());
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
        if (type != 240) {
            transmitMIDI(type, data1, data2, channel, cable);
        } else {
            transmitSysEx(data1 + data2 * 256, usbMIDI.getSysExArray(), cable);
            //Serial.println("DAW Sysex");
        }
    }
}

void transmitMIDI(int t, int d1, int d2, int ch, byte inPort) {
    
    Serial.print("rxMIDI: t");
    Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.print(ch); Serial.print(" inp:"), Serial.println(inPort);
    
    // Normal messages, first we must convert usbMIDI's type (an ordinary
    // byte) to the MIDI library's special MidiType.
    midi::MidiType mtype = (midi::MidiType)t;
    
    // Route to 6 MIDI DIN Ports
    for (int outp = 0; outp < 6; outp++) {
        if (routing[inPort][outp] != 0) {
            switch (outp) {
                case  0: MIDI1.send(mtype, d1, d2, ch); Serial.print("tx m1: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                case  1: MIDI2.send(mtype, d1, d2, ch); Serial.print("tx m2: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                case  2: MIDI3.send(mtype, d1, d2, ch); Serial.print("tx m3: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                case  3: MIDI4.send(mtype, d1, d2, ch); Serial.print("tx m4: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                case  4: MIDI5.send(mtype, d1, d2, ch); Serial.print("tx m5: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                case  5: MIDI6.send(mtype, d1, d2, ch); Serial.print("tx m6: "); Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
                    break;
                    //default:
            }
        }
    }
    
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++) {
        if (routing[inPort][outp] != 0) {
            Serial.print("tx u"); Serial.print(outp - 5); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            midilist[outp - 6]->send(t, d1, d2, ch);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++) {
        if (routing[inPort][outp] != 0) {
            Serial.print("tx h"); Serial.print(outp - 17); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            usbMIDI.send(t, d1, d2, ch, outp - 18);
        }
    }
    
    // Route to 6 CV jacks
    for (int outp = 36; outp < 42; outp++) {
        if (routing[inPort][outp] != 0) {
            int curDac = outp - 36;
            if (curDac < 4) {                     // A1-4 jacks get pitch for note on/off events, with gates going to the D1-4 jacks
                if (t == 144) {  // note on
                    digitalWriteFast(dig1 + (outp - 36), HIGH);     // GATE on
                    setDAC(curDac, CVnoteCal(d1, curDac));     // send note value to CV, -5 to 5v
                } else if (t == 128) {  // note off
                    digitalWriteFast(dig1 + (outp - 36), LOW); // GATE off
                }
            } else if (curDac == 4) {             // A5 gets velocity, D5 gets clock / 16 that starts/stops with realtime start/stop
                if (t == 250) {  // Realtime Start Transport
                    digitalWriteFast(dig5, HIGH);  // pulse on at start
                } else if (t == 248 and startCount < 16) { // clock
                    if (startCount < 15) {
                        startCount++;
                    } else if (startCount == 15) {
                        digitalWriteFast(dig5, LOW);   // pulse off after 16 clocks
                        startCount++;
                    }
                } else if (t == 252) { // Realtime Stop Transport
                    startCount = 0;
                    digitalWriteFast(dig5, LOW);   // pulse off after 16 clocks
                } else if (t == 144) { // note on
                    analogWrite(dac5, CVparamCal(d2, curDac) );  // send velocity to CV5, 0-5v
                }
            } else if (curDac == 5) {             // A6 gets mod wheel, D6 gets clock that starts/stops with realtime start/stop
                if (t == 250) {  // Realtime Start Transport
                    clockPulse = 1;  // set high at start
                } else if (t == 252) { // stop
                    digitalWriteFast(dig6, LOW);
                } else if (t == 248) { // clock
                    digitalWriteFast(dig6, clockPulse);  // clock!!
                    clockPulse = !clockPulse;  // toggle for next clock pulse
                } else if (t == 176 and d1 == 1) { // mod wheel
                    analogWrite(dac6, CVparamCal(d2, curDac) );  // send mod wheel to CV6, 0-5v
                }
            }
        }
    }
}

void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort) {
    Serial.print("rxSysex: len: ");
    Serial.print(len); Serial.print(" array: ");
    
    for (unsigned int i = 0; i < len; i++) {
        Serial.print(sysexarray[i]); Serial.print(", ");
    }
    Serial.print(" inp:"), Serial.println(inPort);
    
    // =======================================================================
    // Before we route the received sysex to an output...
    // =======================================================================
    // ...parse Universal sysex messages that we want to do something with
    if (sysexarray[0] == 240 && sysexarray[1] == 126 && sysexarray[3] == 6 && sysexarray[4] == 2) { // SysExID reply
        if (sysexarray[5] != 0) {
            matchSysExID(sysexarray[5], 0, 0);  // single byte ID
        } else {
            matchSysExID(0, sysexarray[6], sysexarray[7]);  // three byte ID
        }
        
        String rc = mfg;                          // consider optimizing / removing string type
        Serial.print("Rc: "); Serial.println(rc);
        const char * shortName = rc.substring(0, 6).c_str();
        
        MRInputPort *inport = router.inputAt(inPort);
        MROutputPort *outport = router.outputAt(inPort);
        strncpy(inport->name, shortName, 6);
        strncpy(outport->name, shortName, 6);
        //    inputNames[inPort] = rc.substring(0, 6);    // shorten mfg name
        //    outputNames[inPort] = rc.substring(0, 6);
        rdFlag = 1; // flag to redraw screen
    }
    // =======================================================================
    
    // Route to 6 MIDI DIN Ports
    for (int outp = 0; outp < 6; outp++) {
        if (routing[inPort][outp] != 0) {
            switch (outp) {
                case  0: MIDI1.sendSysEx(len, sysexarray, true); break;
                case  1: MIDI2.sendSysEx(len, sysexarray, true); break;
                case  2: MIDI3.sendSysEx(len, sysexarray, true); break;
                case  3: MIDI4.sendSysEx(len, sysexarray, true); break;
                case  4: MIDI5.sendSysEx(len, sysexarray, true); break;
                case  5: MIDI6.sendSysEx(len, sysexarray, true); break;
                    //default:
            }
        }
    }
    
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++) {
        if (routing[inPort][outp] != 0) {
            midilist[outp - 6]->sendSysEx(len, sysexarray, true);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++) {
        if (routing[inPort][outp] != 0) {
            usbMIDI.sendSysEx(len, sysexarray, true, outp - 18);
        }
    }
}

float CVnoteCal(int note, int dac) {
    float cvrange = (dacPos[dac] - dacNeg[dac]);
    cvee = ((note * (cvrange / 120)) + dacNeg[dac]);
    return (cvee);
    //analogWrite(dac5, d1 * 34.133);
    //analogWrite(dac6, d1 * 34.133);
}

float CVparamCal(int data, int dac) {
    float cvrange = ((dacPos[dac] - dacNeg[dac]) / 2);
    cvee = ((data * (cvrange / 127)) + dacNeg[dac]) + cvrange;
    return (cvee);
}

void showADC() {
    //Serial.print("ADC1: "); Serial.print(analogRead(adc1)); Serial.print(" ADC2: "); Serial.println(analogRead(adc2));
}

void profileInstruments() {
    // send Sysex ID requests to all DIN and USB MIDI devices (not to DAW)
    MIDI1.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    MIDI2.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    MIDI3.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    MIDI4.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    MIDI5.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    MIDI6.sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    
    routeMidi();
    
    // detect USB devices
    for (int i = 0; i < 10; i++) {
        String prod = (const char *)midilist[i]->product();
        if (prod != "") {
            Serial.print("USB Device detected: "); Serial.println((const char *)midilist[i]->product());
            
            char portIndex = (char)(i + 6);
            const char * shortName = prod.substring(0, 6).c_str();
            
            MRInputPort *inport = router.inputAt(portIndex);
            MROutputPort *outport = router.outputAt(portIndex);
            strncpy(inport->name, shortName, 6);
            strncpy(outport->name, shortName, 6);
        }
    }
}

#endif /* MR_MIDI_h */
