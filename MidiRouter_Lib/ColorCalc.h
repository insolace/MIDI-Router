//
//  ColorCalc.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef ColorCalc_h
#define ColorCalc_h

class RGBColor {
public:
    
    RGBColor(uint8_t r, uint8_t g, uint8_t b) {
        _r = r;
        _g = g;
        _b = b;
    }
    
    uint16_t asUint16() {
        return ((_r & 0xF8) << 8) | ((_g & 0xFC) << 3) | (_b >> 3);
    }
    
    
private:
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
};

#endif /* ColorCalc_h */
