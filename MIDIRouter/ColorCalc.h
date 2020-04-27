/*

MIDI Router
Created by Eric Bateman (eric at timeline85 dot com)
http://www.midirouter.com
 
ColorCalc.h - MIDI Router color calculations
Source code written by Kurt Arnlund
Copyright © Kurt Arnlund. All rights reserved.

*/


#ifndef ColorCalc_h
#define ColorCalc_h

/// RGBColor object so that color descriptions can be allocated only once
class RGBColor {
public:
    
    /// Constructor
    /// @param r  red 0-255
    /// @param g  green 0-255
    /// @param b  blue 0-255
    RGBColor(uint8_t r, uint8_t g, uint8_t b) {
        _r = r;
        _g = g;
        _b = b;
    }
    
    /// @return 16 bit color
    uint16_t asUint16() {
        return ((_r & 0xF8) << 8) | ((_g & 0xFC) << 3) | (_b >> 3);
    }
    
private:
    uint8_t _r;
    uint8_t _g;
    uint8_t _b;
};

#endif /* ColorCalc_h */
