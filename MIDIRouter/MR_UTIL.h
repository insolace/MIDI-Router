/*

    MIDI Router
    Created by Eric Bateman (eric at timeline85 dot com)
    http://www.midirouter.com

    MR_UTIL.h - MIDI Router utility functions
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

#ifndef MR_UTIL_h
#define MR_UTIL_h

// SD card utilities

// ================================================================================
// SysEx ID lookup
// ================================================================================

void matchSysExID(int16_t b1, int16_t b2, int16_t b3)
{
    SysCsvFile.seek(156); // skip first 3 lines of Sysex Table

    while (SysCsvFile.available())
    {

        if (csvReadText(&SysCsvFile, syIdHex, sizeof(syIdHex), CSV_DELIM) != CSV_DELIM
                || csvReadText(&SysCsvFile, mfg, sizeof(mfg), CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idLen, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB1, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB2, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB3, CSV_DELIM) != CSV_DELIM)
        {
            Serial.println("read error");
            Serial.print("Pos: "); Serial.println(SysCsvFile.position());
            int ch;
            int nr = 0;
            // print part of file after error.
            while ((ch = SysCsvFile.read()) > 0 && nr++ < 100)
            {
                Serial.write(ch);
                Serial.print("("); Serial.print(nr); Serial.print(")");
            }
            break;
        }
        if ((b1 == 0 && idB1 == 0 && idB2 == b2 && idB3 == b3)
                || ((b1 != 0) && (b1 == idB1)))
        {
            printMatch();
            break;
        }

    }
}

void printMatch()
{
    Serial.print("MATCH: ");
    Serial.print(syIdHex);
    Serial.print(CSV_DELIM);
    Serial.print(mfg);
    Serial.print(CSV_DELIM);
    Serial.print(idLen);
    Serial.print(CSV_DELIM);
    Serial.print(idB1);
    Serial.print(CSV_DELIM);
    Serial.print(idB2);
    Serial.print(CSV_DELIM);
    Serial.println(idB3);
}

// ================================================================================
// CSV
// ================================================================================


int csvReadText(File* SysCsvFile, char* str, size_t size, char delim)
{
    char ch;
    int rtn;
    size_t n = 0;
    while (true)
    {
        // check for EOF
        if (!SysCsvFile->available())
        {
            rtn = 0;
            break;
        }
        if (SysCsvFile->read(&ch, 1) != 1)
        {
            // read error
            rtn = -1;
            break;
        }
        // Delete CR.
        if (ch == '\r')
        {
            continue;
        }
        if (ch == delim || ch == '\n')
        {
            rtn = ch;
            break;
        }
        if ((n + 1) >= size)
        {
            // string too long
            rtn = -2;
            n--;
            break;
        }
        str[n++] = ch;
    }
    str[n] = '\0';
    return rtn;
}

// MARK: - CSV Reading

int csvReadInt32(File* SysCsvFile, int32_t* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtol(buf, &ptr, 10);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadInt16(File* SysCsvFile, int16_t* num, char delim)
{
    int32_t tmp;
    int rtn = csvReadInt32(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    if (tmp < INT_MIN || tmp > INT_MAX)
    {
        return -5;
    }
    *num = tmp;
    if (rtn == 10)
    {
        return 44;    // convert NL to comma
    }
    return rtn;
}

int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtoul(buf, &ptr, 10);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim)
{
    uint32_t tmp;
    int rtn = csvReadUint32(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    if (tmp > UINT_MAX)
    {
        return -5;
    }
    *num = tmp;
    return rtn;
}

int csvReadDouble(File* SysCsvFile, double* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtod(buf, &ptr);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadFloat(File* SysCsvFile, float* num, char delim)
{
    double tmp;
    int rtn = csvReadDouble(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    // could test for too large.
    *num = tmp;
    return rtn;
}

void csvClose()
{
    SysCsvFile.close();
}

// ================================================================================
// Re-order routing filter sequence
// ================================================================================

int reOrderR(int r)
{
    switch (r)                            // re-order the routing selection in a way that prioritizes early selection of common filtering
    {
        case  0: return B00000000;  // ___
        case  1: return B00000111;  // KPT
        case  2: return B00000100;  // __T
        case  3: return B00000001;  // K__
        case  4: return B00000011;  // KP_
        case  5: return B00000101;  // K_T
        case  6: return B00000110;  // _PT
        case  7: return B00000010;  // _P_


        default: return 0;
    }
}

// print font character widths to serial port
void printCharWidths()
{
    tft.setCursor(0,0);
    int16_t ascX = 0, ascY = 0;
    int aCount = 0;
    int lastaX = 0;
    for (int i = 32; i < 127; i++)
    {
        tft.print(char(i));
        tft.getCursor(ascX, ascY);
        Serial.print(ascX - lastaX); Serial.print(", ");
        lastaX = ascX;
        if (aCount > 20) {
            tft.print("\n");
            aCount = 0;
            lastaX = 0;
        }
        aCount++;
    }
}


#endif /* MR_UTIL_h */
