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
    int len = s.length() + 1;
    byte buff[len];
    s.getBytes(buff, len);
    for (int i = 0; i < len - 1; i++)
    {
        dWrite(buff[i], sz);
    }
}

void dWrite(unsigned char c, unsigned int s)
{
    //curX = tft.getCursorX();
    //curY = tft.getCursorY();
    //Serial.println(c);
    //Serial.println(curX);
    //Serial.println(curY);

    //if (curX+fWidth > WIDE) { curX = 0; curY = curY + fHeight; }  // wrap text horizontal
    //if (curY+fHeight > TALL) { curY = 0; curX = 0; }              // wrap text vertical

    tft.graphicsMode();
    tft.drawChar(curX, curY, c, fColor, fBG, s);

    curX = curX + (s * 6) + 1;

}

/*

    // code below not used, for reference only

    void textSubs() {
    //testRot();

    tft.graphicsMode();
    tft.setRotation(2);
    //tft.fillRect(11, 11, 398, 198, RA8875_BLUE);

    tft.setCursor(10,10);
    tft.drawChar(10, 10, 'E', 0xFFFF, 0x0000, 2);

    tft.setTextRotation(1);

    tft.textMode();
    tft.textColor(RA8875_RED, RA8875_BLACK);
    tft.textSetCursor(0,16);
    tft.textEnlarge(3);
    tft.textWrite("HELLOWORLD");


    delay(2000);
    tft.fillScreen(RA8875_BLACK);
    delay(1000);
    tft.fillScreen(RA8875_YELLOW);
    delay(1000);

    tft.graphicsMode();
    tft.setRotation(1);
    //tft.fillRect(11, 11, 398, 198, RA8875_BLUE);


    tft.setCursor(10,10);
    tft.drawChar(10, 10, 'E', 0xFFFF, 0x0000, 2);

    tft.setTextRotation(0);

    tft.textMode();
    tft.textColor(RA8875_RED, RA8875_BLACK);
    tft.textSetCursor(0,16);
    tft.textEnlarge(3);
    tft.textWrite("HELLOWORLD");

    //tft.fillRect(11, 11, 398, 198, RA8875_BLUE);
    delay(2000);
    tft.fillScreen(RA8875_BLACK);
    delay(1000);
    tft.fillScreen(RA8875_YELLOW);
    delay(1000);

    }

*/

#endif /* MR_TXT_h */
