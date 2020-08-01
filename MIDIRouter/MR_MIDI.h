/*
 
 MIDI Router
 Created by Eric Bateman (eric at timeline85 dot com)
 http://www.midirouter.com
 
 MR_MIDI.h - MIDI Router MIDI processing
 Source code written by Eric Bateman with contributions from Kurt Arnlund
 Copyright © 2020 Eric Bateman and Kurt Arnlund. All rights reserved.
 
 License:GNU General Public License v3.0
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MR_MIDI_h
#define MR_MIDI_h

void routeMidi()
{
    // ============================================================
    // RX Hardware DIN MIDI ports
    // ============================================================
    
    // Read messages arriving from the 6 hardware DIN ports

    for (int port = 0; port < 6; port++)
    {
        if (dinlist[port]->read())
        {
            byte type =          dinlist[port]->getType();
            byte data1 =         dinlist[port]->getData1();
            byte data2 =         dinlist[port]->getData2();
            byte channel =       dinlist[port]->getChannel();
            const uint8_t *sys = dinlist[port]->getSysExArray();
            if (type != 240)
            {
                transmitMIDI(type, data1, data2, channel, port);
            }
            else
            {
                transmitSysEx(data1 + data2 * 256, sys, port);
            }
        }
    }
    // ============================================================
    // RX USB DEVICE MIDI ports
    // ============================================================
    
    // Next read messages arriving from the (up to) 10 USB devices plugged into the MIDIROUTER USB devices port
    for (int port = 0; port < 10; port++)
    {
        if (midilist[port]->read())
        {
            uint8_t type =       midilist[port]->getType();
            uint8_t data1 =      midilist[port]->getData1();
            uint8_t data2 =      midilist[port]->getData2();
            uint8_t channel =    midilist[port]->getChannel();
            const uint8_t *sys = midilist[port]->getSysExArray();
            if (type != 240)
            {
                transmitMIDI(type, data1, data2, channel, port + 6);
            }
            else
            {
                transmitSysEx(data1 + data2 * 256, sys, port + 6);
            }
        }
    }
    
    // ============================================================
    // RX USB HOST MIDI ports
    // ============================================================
    
    // Read USB HOST (computer/DAW) messages the PC sends to Teensy, 16 ports. Reroute them based on matrix
    
    if (usbMIDI.read())
    {
        // get the USB MIDI message from computer, defined by these 5 numbers (except SysEX)
        byte type = usbMIDI.getType();
        byte data1 = usbMIDI.getData1();
        byte data2 = usbMIDI.getData2();
        byte channel = usbMIDI.getChannel();
        byte cable = usbMIDI.getCable() + 18; // 18 is offset for USB Host
        if (type != 240)
        {
            transmitMIDI(type, data1, data2, channel, cable);
        }
        else
        {
            transmitSysEx(data1 + data2 * 256, usbMIDI.getSysExArray(), cable);
            //Serial.println("DAW Sysex");
        }
    }
}

void transmitMIDI(int t, int d1, int d2, int ch, byte inPort)
{
    volatile int cR = 0;
    Serial.print("rxMIDI: t"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.print(ch); Serial.print(" inp:"), Serial.println(inPort);
    
    // Normal messages, first we must convert usbMIDI's type (an ordinary
    // byte) to the MIDI library's special MidiType.
    midi::MidiType mtype = (midi::MidiType)t;
    
    // Route to 6 MIDI DIN Ports
    for (int outp = 0; outp < 6; outp++)
    {
        cR = routing[inPort][outp];
        if (cR != 0)
        {
            if (filtRoute(mtype, ch, cR))
            {
                dinlist[outp]->send(mtype, d1, d2, ch);
            }
            Serial.print("tx d"); Serial.print(outp - 5); Serial.print(":");Serial.print(mtype); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
        }
    }
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++)
    {
        cR = routing[inPort][outp];
        if (filtRoute(t, ch, cR))
        {
            Serial.print("tx u"); Serial.print(outp - 5); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            midilist[outp - 6]->send(t, d1, d2, ch);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++)
    {
        cR = routing[inPort][outp];
        if (filtRoute(t, ch, cR))
        {
            Serial.print("tx h"); Serial.print(outp - 17); Serial.print(":"); Serial.print(t); Serial.print(" d1:"), Serial.print(d1); Serial.print(" d2:"), Serial.print(d2); Serial.print(" ch:"), Serial.println(ch);
            usbMIDI.send(t, d1, d2, ch, outp - 18);
        }
    }
    
    // Route to 6 CV jacks
    for (int outp = 36; outp < 42; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            int curDac = outp - 36;
            cR = routing[inPort][outp];
            
            // test channel against filter, if not 0 or no match then exit
            int filtChan = cR >> 3;
            int filtType = cR & B00000111; // gather current filter setting
            
            if (filtChan == ch || filtChan == 0) // match channel
            {
                if (filtType == cvF_NOT || filtType == cvF_VEL) {                         // note on/off and/or velocity
                    if (t == 0x90) // note on (vel 0 = note off)
                    {
                        if (d2 == 0) // note off
                        {
                            setDIG(curDac, LOW); // GATE off
                        } else { // note on
                            if (filtType == cvF_NOT)
                            {
                                setDAC(curDac, CVnoteCal(d1, curDac)); // pitch of the note
                            } else {
                                setDAC(curDac, CVparamCal(d2, curDac)); // velocity
                            }
                            setDIG(curDac, HIGH); // GATE on
                            
                        }
                    } else if (t == 0x80) {
                        setDIG(curDac, LOW); // GATE off
                    }
                } else if (t == 0xE0 && filtType == cvF_PW) {                   // Pitch Wheel
                    int16_t pw14 = (d2 << 7 | d1);
                    setDAC(curDac, CV14bitCal( pw14, curDac)); // 14 bit converted to 16 bit value
                } else if (t == 0xB0 && d1 == 1 && filtType == cvF_MOD) {       // Mod Wheel
                    setDAC(curDac, CVparamCal(d2, curDac)); // mod value
                } else if (t == 0xB0 && filtType == cvF_CCs) {                  // CCs (any)
                    setDAC(curDac, CVparamCal(d2, curDac)); // CC value
                } else if (t == 0xB0 && d1 == 74 && filtType == cvF_CC74) {     // CC74 (MPE Y axis)
                    setDAC(curDac, CVparamCal(d2, curDac)); // MPE Y axis value
                } else if (t == 0xA0 && filtType == cvF_AT) {                   // AT (poly, any key)
                    setDAC(curDac, CVparamCal(d2, curDac)); // polyAT value, any key
                } else if (t == 0xD0 && filtType == cvF_AT) {                   // AT (channel)
                    setDAC(curDac, CVparamCal(d1, curDac)); // chanAT value
                }
                
            }
            if (filtType > 2) // clock out for any filter other than note/velocity
            {
                if (t == 250) {   // Realtime Start Transport
                    clockPulse[curDac] = 1;  // set high at start
                } else if (t == 252) {    // stop
                    setDIG(curDac, LOW);
                }
                else if (t == 248)     // clock
                {
                    setDIG(curDac, clockPulse[curDac]);  // clock!!
                    clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                }
            }
            
            
            /*
            if (curDac < 4)                       // A1-4 jacks get pitch for note on/off events, with gates going to the D1-4 jacks
            {
                if (t == 144)    // note on
                {
                    digitalWriteFast(dig1 + (outp - 36), HIGH);     // GATE on
                    setDAC(curDac, CVnoteCal(d1, curDac));     // send note value to CV, -5 to 5v
                }
                else if (t == 128)      // note off
                {
                    digitalWriteFast(dig1 + (outp - 36), LOW); // GATE off
                }
            }
            else if (curDac == 4)                 // A5 gets velocity, D5 gets clock / 16 that starts/stops with realtime start/stop
            {
                if (t == 250)    // Realtime Start Transport
                {
                    digitalWriteFast(dig5, HIGH);  // pulse on at start
                }
                else if (t == 248 and startCount < 16)     // clock
                {
                    if (startCount < 15)
                    {
                        startCount++;
                    }
                    else if (startCount == 15)
                    {
                        setDIG(curDac, clockPulse[curDac]);  // clock!!
                        clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                        startCount = 0; // was startCount++ EB TODO: Verify this!
                    }
                }
                else if (t == 252)     // Realtime Stop Transport
                {
                    startCount = 0;
                    digitalWriteFast(dig5, LOW);   // pulse off after 16 clocks
                }
                else if (t == 144)     // note on
                {
                    setDAC(curDac, CVparamCal(d2, curDac));   // send velocity to CV5, 0-5v
                }
            }
            else if (curDac == 5)                 // A6 gets mod wheel, D6 gets clock that starts/stops with realtime start/stop
            {
                if (t == 250)    // Realtime Start Transport
                {
                    clockPulse = 1;  // set high at start
                }
                else if (t == 252)     // stop
                {
                    setDIG(curDac, LOW);
                }
                else if (t == 248)     // clock
                {
                    setDIG(curDac, clockPulse[curDac]);  // clock!!
                    clockPulse[curDac] = !clockPulse[curDac];  // toggle for next clock pulse
                }
                else if (t == 176 and d1 == 1)     // mod wheel
                {
                    setDAC(curDac, CVparamCal(d2, curDac));   // send mod wheel to CV6, 0-5v
                }
            }*/
        }
    }
}

bool filtRoute(int t, int ch, int f)   // evaluate MIDI type and ch against filter setting
{
    int filtChan = f >> 3;
    //Serial.print("filtChan:"); Serial.print(filtChan); Serial.print(" ch:"); Serial.println(ch);
    if ((t == 0x80) || (t == 0x90) || (t == 0xA0) || (t == 0xD0) || (t == 0xE0))             // note on, note off, polyAT, chanAT, pitch bend
    {
        if (f & B00000001 && (filtChan == ch || filtChan == 0))
        {
            return true;    // keyboard events routed
        }
    }
    else if ((t == 0xB0) || (t == 0xC0) || (t == 0xF3) || (t == 0xF0) || (t == 0xF7))        // control change, program change, song select, SysEx
    {
        if (f & B00000010 && (filtChan == ch || filtChan == 0))
        {
            return true;    // parameter events routed
        }
    }
    else if (t)                                                                          // common and realtime (transport/clock) events
    {
        if (f & B00000100)
        {
            return true;    // transport/clock events routed
        }
    }
    return false;
}

/*
int16_t filtCV(int t, int16_t d1, int16_t d2, int ch, int f) // filter messages against CV routing
{
    // test channel against filter, if not 0 or no match then exit
    int filtChan = f >> 3;
    if (filtChan != ch || filtChan != 0) {return -1;} // no ch match
    
    int filtType = f & B00000111; // gather current filter setting
    
    if (t == 0x90 && filtType == cvF_NOT) {                         // note on (ignore noteoff)
        setDAC(curDac, CVnoteCal(d1, curDac)); // pitch of the note
    } else if (t == 0x90 && filtType == cvF_VEL) {                  // velocity
        setDAC(curDac, CVparamCal(d2, curDac)); // velocity
    } else if (t == 0xE0 && filtType == cvF_PW) {                   // Pitch Wheel
        setDAC(curDac, CV14bitCal( (d1 << 8 | d2), curDac)); // 14 bit converted to 16 bit value
    } else if (t == 0xB0 && d1 == 1 && filtType == cvF_MOD) {       // Mod Wheel
        setDAC(curDac, CVparamCal(d2, curDac)); // mod value
    } else if (t == 0xB0 && filtType == cvF_CCs) {                  // CCs (any)
        setDAC(curDac, CVparamCal(d2, curDac)); // CC value
    } else if (t == 0xB0 && D1 == 74 && filtType == cvF_CC74) {     // CC74 (MPE Y axis)
        setDAC(curDac, CVparamCal(d2, curDac)); // MPE Y axis value
    } else if (t == 0xA0 && filtType == cvF_AT) {                   // AT (poly, any key)
        setDAC(curDac, CVparamCal(d2, curDac)); // polyAT value, any key
    } else if (t == 0xD0 && filtType == cvF_AT) {                   // AT (channel)
        setDAC(curDac, CVparamCal(d1, curDac)); // chanAT value
    } else {
        return -1; // no match
    }
} */

void transmitSysEx(unsigned int len, const uint8_t *sysexarray, byte inPort)
{
    Serial.print("rxSysex: len: ");
    Serial.print(len); Serial.print(" array: ");
    
    for (unsigned int i = 0; i < len; i++)
    {
        Serial.print(sysexarray[i]); Serial.print(", ");
    }
    Serial.print(" inp:"), Serial.println(inPort);
    
    // =======================================================================
    // Before we route the received sysex to an output...
    // =======================================================================
    // ...parse Universal sysex messages that we want to do something with
    if (sysexarray[0] == 240 && sysexarray[1] == 126 && sysexarray[3] == 6 && sysexarray[4] == 2)   // SysExID reply
    {
        if (sysexarray[5] != 0)
        {
            matchSysExID(sysexarray[5], 0, 0);  // single byte ID
        }
        else
        {
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
    for (int outp = 0; outp < 6; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            dinlist[outp]->sendSysEx(len, sysexarray, true);
        }
    }
    
    // Route to 10 USB Devices
    for (int outp = 6; outp < 16; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            midilist[outp - 6]->sendSysEx(len, sysexarray, true);
        }
    }
    
    // Route to 16 USB Host Ports
    for (int outp = 18; outp < 34; outp++)
    {
        if (routing[inPort][outp] != 0)
        {
            usbMIDI.sendSysEx(len, sysexarray, true, outp - 18);
        }
    }
}

float CVnoteCal(int note, int dac)
{
    float cvrange = (dacPos[dac] - dacNeg[dac]);
    return ((note * (cvrange / 120)) + dacNeg[dac]);
    
}

float CVparamCal(int data, int dac)
{
    float cvrange = ((dacPos[dac] - dacNeg[dac]) / 2);
    return (((data * (cvrange / 127)) + dacNeg[dac]) + cvrange);
}

float CV14bitCal(int16_t data, int dac)
{
    float cvrange = ((dacPos[dac] - dacNeg[dac]));
    return (((data * (cvrange / 16383)) + dacNeg[dac]));
}

void showADC()
{
    Serial.print("ADC1: "); Serial.print(analogRead(adc1)); Serial.print(" ADC2: "); Serial.println(analogRead(adc2));
}

void profileInstruments()
{
    // send Sysex ID requests to all DIN and USB MIDI devices (not to DAW)
    for (int i = 0; i < 6; i++)
    {
        dinlist[i]->sendSysEx(sizeof(sysexIDReq), sysexIDReq, true); // request Sysex ID
    }
    updateUSB();
}

void updateUSB()
{
    // gather status
    //Serial.println("updateUSB");
    for (int i = 0; i < 10; i++)
    {
        //Serial.print("U: "); Serial.println(i);
        if (usbWasActive[i] != *midilist[i])
        {
            int tst = *midilist[i];
            //Serial.print("ML: "); Serial.println(tst);
            //Serial.print("Change USB"); Serial.println(i+1);
            
            String prod = (const char *)midilist[i]->product();
            
            if (prod == "") {
                prod = "USB ";
                prod.append(i+1);
            }
            Serial.print("USB Device detected: "); Serial.println(prod);

            char portIndex = (char)(i + 6);
            const char * shortName = prod.substring(0, 9).c_str();
            //const char * medName = prod.substring(0, 9).c_str();
            
            MRInputPort *inport = router.inputAt(portIndex);
            MROutputPort *outport = router.outputAt(portIndex);
            strncpy(inport->name, shortName, 6);
            strncpy(outport->name, shortName, 8);
            
            usbWasActive[i] = tst;
            
            drawColumns();
            drawRows();
        }
    }
}

#endif /* MR_MIDI_h */
