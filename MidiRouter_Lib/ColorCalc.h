//
//  ColorCalc.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef ColorCalc_h
#define ColorCalc_h

uint16_t newColor(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#endif /* ColorCalc_h */
