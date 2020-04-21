//
//  MR_TXT.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef MR_TXT_h
#define MR_TXT_h

void dPrint(String s) {  // adapted from PJRC's Print.cpp
    int len = s.length()+1;
    byte buff[len];
    s.getBytes(buff, len);
    for (int i = 0; i<len-1; i++) {
        dWrite(buff[i]);
    }
}

void dWrite(unsigned char c) {
    //curX = tft.getCursorX();
    //curY = tft.getCursorY();
    //Serial.println(c);
    //Serial.println(curX);
    //Serial.println(curY);
    
    //if (curX+fWidth > WIDE) { curX = 0; curY = curY + fHeight; }  // wrap text horizontal
    //if (curY+fHeight > TALL) { curY = 0; curX = 0; }              // wrap text vertical
    tft.graphicsMode();
    tft.drawChar(curX, curY, c, 0xFFFF, 0, fSize);
    
    curX = curX + (fSize*6) + 1;
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
