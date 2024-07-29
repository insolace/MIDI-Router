#include "cvDAC.hpp"
#include <stdint.h>
#include "SPI.h"

// DAC stuff
long dacNeg[6]; ///< Calibrated value for -5VDC at dac output
long dacPos[6]; ///< Calibrated value for +5VDC at dac output
long dacOffset[120]; ///< Stored array for custom offsets per note (not yet implemented)

uint16_t posCol;  // for CV calib
uint16_t negCol;  // for CV calib

void initDAC()
{
	   // ============================================================
    // DAC 16bit SPI setup
    // ============================================================

    // set the CS as an output:
    pinMode(CS, OUTPUT);
    digitalWrite(CS, LOW);

    // initialise SPI:
    SPI2.begin();
    SPI2.setBitOrder(MSBFIRST);

    //enable internal ref
    digitalWriteFast(CS, LOW);
    int c1 = B00000001;
    int c2 = B00010000;
    int c3 = B00000000;
    SPI2.transfer(c1);
    SPI2.transfer(c2);
    SPI2.transfer(c3);
    digitalWriteFast(CS, HIGH);

    setDAC(6, dacNeg[0]);

    // ============================================================
    // DAC 12bit internal setup
    // ============================================================

    pinMode(dac5, OUTPUT);
    pinMode(dac6, OUTPUT);
    analogWriteResolution(12);

    // ============================================================
    // D 1-6 setup
    // ============================================================
    pinMode(dig1, OUTPUT);
    pinMode(dig2, OUTPUT);
    pinMode(dig3, OUTPUT);
    pinMode(dig4, OUTPUT);
    pinMode(dig5, OUTPUT);
    pinMode(dig6, OUTPUT);
    //pinMode(dig1, OUTPUT);
    //pinMode(adc1, INPUT);  // hardware is there but not implemented yet in software
    //pinMode(adc2, INPUT);


}

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

void setDIG(int dig, bool s) // iterated adjustment of eurorack digital outs
{
    switch (dig)
    {
        case 0:
            dig = dig1;
            break;
        case 1:
            dig = dig2;
            break;
        case 2:
            dig = dig3;
            break;
        case 3:
            dig = dig4;
            break;
        case 4:
            dig = dig5;
            break;
        case 5:
            dig = dig6;
            break;
    }
    digitalWriteFast(dig, s);
}