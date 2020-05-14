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

#ifndef MR_DAC_h
#define MR_DAC_h

void setDAC(int dac, uint32_t data)
{
    int c1 = 0, c2 = 0, c3 = 0;
    /*  fix incorrect prototype wiring
        dacA = A3
        dacB = A2
        dacC = A4
        dacD = A1  */
    switch (dac)
    {
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
