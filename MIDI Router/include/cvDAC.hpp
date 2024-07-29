/*
    MIDI Router
    Created by Eric Bateman (eric at timeline85 dot com)
    http://www.midirouter.com

    MR_DAC.h - MIDI Router EURORACK functions
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

#ifndef CVDAC_H
#define CVDAC_H

#include "binary.h"
#include <stdint.h>
#include <Arduino.h>



// DAC 16bit SPI
#define dacA B00010100 ///< DAC 1 address
#define dacB B00010010 ///< DAC 2 address
#define dacC B00010110 ///< DAC 3 address
#define dacD B00010000 ///< DAC 4 address
#define dALL B00110100 ///< DAC ALL address

#define CS 43   ///< SPI chip select pin

// DAC 12bit internal
#define dac5 A22    ///< DAC 5 pin
#define dac6 A21    ///< DAC 6 pin

// DOUT Gate 1-6 setup
#define dig1 3      ///< DOUT 1 pin
#define dig2 4      ///< DOUT 2 pin
#define dig3 5      ///< DOUT 3 pin
#define dig4 6      ///< DOUT 4 pin
#define dig5 22     ///< DOUT 5 pin
#define dig6 21     ///< DOUT 6 pin
#define adc1 A9     ///< Analog to Digital Converter 1 pin
#define adc2 A6     ///< Analog to Digital Converter 2 pin


// DAC stuff
extern long dacNeg[6]; ///< Calibrated value for -5VDC at dac output
extern long dacPos[6]; ///< Calibrated value for +5VDC at dac output
extern long dacOffset[120]; ///< Stored array for custom offsets per note (not yet implemented)

extern uint16_t posCol;  // for CV calib
extern uint16_t negCol;  // for CV calib


void initDAC();

/// set DAC output value
/// @param dac DAC identifier
/// @param data 16 bit output value for DAC1-4, 12 bit output value for DAC5-6
void setDAC(int dac, uint32_t data);

/// set DIG output value
/// @param dac DAC identifier
/// @param s HIGH/LOW value to set
void setDIG(int dig, bool s);


#endif/* CVDAC_H */
