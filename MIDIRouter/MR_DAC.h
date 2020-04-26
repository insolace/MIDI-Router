//
//  MR_DAC.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef MR_DAC_h
#define MR_DAC_h

void setDAC(int dac, uint32_t data) {
    int c1 = 0, c2 = 0, c3 = 0;
    /* fix incorrect prototype wiring
     dacA = A3
     dacB = A2
     dacC = A4
     dacD = A1  */
    switch (dac) {
        case 0:
            c1 = dacD;
            break;
        case 1:
            c1 = dacB;
            break;
        case 2:
            c1 = dacA;
            break;
        case 3:
            c1 = dacC;
            break;
        case 4:
            analogWrite(dac5, data);
            return;
            break;
        case 5:
            analogWrite(dac6, data);
            return;
            break;
        case 6:
            c1 = dALL;
            analogWrite(dac5, data);
            analogWrite(dac6, data);
            break;
    }
    
    digitalWriteFast(CS, LOW);
    
    c2 = data >> 8;
    c3 = data;
    
    SPI2.transfer(c1);
    SPI2.transfer(c2);
    SPI2.transfer(c3);
    
    digitalWriteFast(CS, HIGH);
}

#endif /* MR_DAC_h */
