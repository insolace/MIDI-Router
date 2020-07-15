/*
 
 MIDI Router
 Created by Eric Bateman (eric at timeline85 dot com)
 http://www.midirouter.com
 
 MR_TXT.h - MIDI Router text functions
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

#ifndef MR_TXT_h
#define MR_TXT_h

void dPrint(String s, int sz)
{
    int len = s.length()+1;
    byte buff[len];
    s.getBytes(buff, len);
    for (int i = 0; i<len-1; i++) {
        dWrite(buff[i], sz);
    }
}

void dWrite(unsigned char c, unsigned int s)
{
    tft.setFontScale(s);
    tft.print("c");
}

void printVert(String s)
{
    int x = tft.getCursorX();
    int y = tft.getCursorY();
    for (int i = 0; i < s.length(); i++)
    {
        tft.print(s.charAt(i));
        y = y + tft.getFontHeight();
        tft.setCursor(x, y);
    }
}

void fontSize(enum RA8875tsize ts)
{
    pfSize = fSize;
    fSize = ts;
    tft.setFontSize(ts);
}

void printCenter(String s, int x, int y, enum RA8875tsize ts)
{
    fontSize(ts); // set the current font size to ts
    switch (ts)
    {
        case X16:
            tft.setCursor(x - (getWidthAX24(s, arial16CW))/2, y);
            break;
        case X24:
            tft.setCursor(x - (getWidthAX24(s, arial24CW))/2, y);
            break;
        case X32:
            tft.setCursor(x - (getWidthAX24(s, arial32CW))/2, y);
            break;
    }
    tft.print(s);
    fontSize(pfSize); // restore font size to previous
}

int getWidthAX24(String s, int * wArray)
{
    int sLen = 0;
    for (int i = 0; i < s.length(); i++)
    {
        sLen += wArray[(char(s[i])-32)];
    }
    return sLen;
}

#endif /* MR_TXT_h */
