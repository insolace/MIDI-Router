/*
 
 MIDI Router
 Created by Eric Bateman (eric at timeline85 dot com)
 http://www.midirouter.com
 
 MR_DRAW.h - MIDI Router graphics
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

#ifndef MR_DRAW_h
#define MR_DRAW_h

// =============================
// draw globals box
// =============================
void drawBox()
{
    tft.clearScreen(RA8875_BLACK);
    
    // home box background
    bg_homebox.draw(0, 0, false); // draw hbbg
    
    // home icons
    if (menu == 0) { // routing screen
        printCenter("Output PG", (bg_homebox.w*.25), (bg_homebox.h/2) + (tBord/2), X16);
        printCenter("Input PG", (bg_homebox.w*.75), tBord/2, X16);
        ic_routing.draw(0, 0, true);
    } else if (menu == 1) { // cv calibrate screen
        ic_calib_back.draw(0, 0, true); // back icon
        ic_calib_clear.draw(101, 61, true); // clear icon
    }
}


// =============================
// draw rows and columns
// =============================


void drawColumns()
{
    MRInputPort *input;
    uint16_t curColor;
    for (int c = 0; c < columns; c++) {
        if (menu == 0) {  // routing screen
            
            bg_inputs.draw(cOffset + (c * cWidth), 0, false); // background
            
            int p = (pgIn * 10) + c;
            guiElem * icon = &empty;
            
            if (p < 6) {         // pg0, din
                icon = &ic_din;
            } else if (p < 24) { // pg1-2, usb
                icon = &ic_usb;
            } else if (p > 25 && p < 54) { // pg3-5, daw
                icon = &ic_daw;
            }
            
            // draw icon
            icon->draw(cOffset + (c * cWidth) + ((cWidth/2) - (icon->w/2)), 0, true);
            
            // draw name
            tft.setActiveWindow(cOffset + (c * cWidth) + tBord, cOffset + ((c + 1) * cWidth) - tBord + icon->w, tBord, rOffset - tBord );
            input = router.inputAt(c + (pgIn * 6));
            printCenter(input->name, cOffset + (c * cWidth) + (cWidth/2), icon->h + tBord);
            
        } else if (menu == 1) { // cv calibrate screen
            if (c == CVcalSelect)
            {
                curColor = actFieldBg;
            } else {
                curColor = insColor;
            }
            tft.fillRect(cOffset + (cWidth * c) + 1, 0, cWidth - 2, rOffset, curColor);
            
            tft.setTextColor(txColor);
            tft.setActiveWindow(cOffset + (c * cWidth) + tBord, cOffset + ((c + 1) * cWidth) - tBord, tBord, rOffset - tBord );
            
            // router could be used to obtain the CV names here
            //input = router.inputAt((5-i)+(pgIn *6));
            tft.setCursor(cOffset + (c * cWidth) + tBord, tBord);
            tft.print("A");
            tft.print(c + 1);
        }
        tft.setActiveWindow();
    }
}

void drawRows()
{
    MROutputPort *output;
    for (int r = 0; r < 6; r++)
    {
        bg_outputs.draw(0, rOffset + (r * rHeight), false); // background
        
        int p = (pgOut * 10) + r;
        guiElem * icon = &empty;
        
        if (p < 6) {         // pg0, din
            icon = &ic_din;
        } else if (p < 24) { // pg1-2, usb
            icon = &ic_usb;
        } else if (p > 25 && p < 54) { // pg3-5, daw
            icon = &ic_daw;
        } else if (p > 56) {
            icon = &ic_eur;
        }
        
        icon->draw(0, rOffset + (r * rHeight), true); // icon
        
        // draw names
        tft.setCursor(icon->w, rOffset + (r * rHeight) + tROffset);
        output = router.outputAt(r + (pgOut *6));
        tft.print(output->name);
    }
}

void drawRouting() { // draw the 6x6 routing grid
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            drawRoute(c, r);
        }
    }
}

void drawRoute(int c, int r) // c and r = column/row of current displayed grid
{
    curRoute = routing[c + (pgIn * 6)][r + (pgOut * 6)]; // store current routing point
    if ((curRoute & B00000111) != 0) {  // draw routed
        grid_routed.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), false); // routed bg
        
        bool drawCh = 0;
        if (pgOut < 6) // for now evaluate what page we are on, TODO: evaluate based on port properties
        {
            drawCh = drawRouteMidi(c, r);
        } else {
            drawCh = drawRouteCV(c, r);
        }
        if (drawCh == 1)
        {
            drawRouteCH(c, r);
        }
    } else {
        // draw unrouted
        grid_unrouted.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), false);
    }
    
    if ( (curCol - (pgIn * 6) == c) && (curRow - (pgOut * 6)  == r) ) {
        highlightSelect(c, r, 1); // draw green rectangle around current selected route
    }
}

bool drawRouteMidi(int c, int r)
{
    bool drawCh = 0;
    // draw piano
    if (curRoute & B00000001) {
        grid_notes.draw(cOffset + (c * cWidth), rOffset + (r * rHeight) + grid_param.h, true); // draw piano roll
        drawCh = 1;
    }
    // draw parameter
    if (curRoute & B00000010) {
        grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
        drawCh = 1;
    }
    // draw transport
    if (curRoute & B00000100) {
        grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
    }
    return drawCh;
}

void drawRouteCH(int c, int r)
{
    // draw channel if parameter or piano are active
    tft.setTextColor(routColor);
    String chText = "ch ";
    if (curRoute >> 3 != 0)
    {
        uint8_t filtChan = curRoute >> 3; // bits 3-7 are our filtered channel
        chText.append(String(filtChan));
    } else {
        chText.append("*");
    }
    
    printCenter(chText,
                cOffset + (c * cWidth) + (cWidth/2) + (grid_trans.w/2),
                rOffset + (r * rHeight) + grid_param.h + tBord/2, X16);
    tft.setTextColor(txColor);
}

bool drawRouteCV(int c, int r)
{
    bool drawCh = 0;
    int filtType = curRoute & B00000111; // gather current filter setting
    tft.setTextColor(routColor);
    switch (filtType)
    {
        case cvF_NOT: // note - pitch
            grid_notes.draw(cOffset + (c * cWidth), rOffset + (r * rHeight) + grid_param.h, true); // draw piano roll
            drawCh = 1;
            printCenter("Note",
                        cOffset + (c * cWidth) + (grid_notes.w /2),
                        rOffset + (r * rHeight) + tBord, X16);
            printCenter("Gate",
                        cOffset + (c * cWidth) + (cWidth/2) + (grid_trans.w/2),
                        rOffset + (r * rHeight) + tBord, X16);
            break;
        case cvF_VEL: // note - velocity
            grid_notes.draw(cOffset + (c * cWidth), rOffset + (r * rHeight) + grid_param.h, true); // draw piano roll
            drawCh = 1;
            printCenter("Vel",
                        cOffset + (c * cWidth) + (grid_notes.w /2),
                        rOffset + (r * rHeight) + tBord, X16);
            printCenter("Gate",
            cOffset + (c * cWidth) + (cWidth/2) + (grid_trans.w/2),
            rOffset + (r * rHeight) + tBord, X16);
            break;
        case cvF_PW: // pitch wheel
            grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
            drawCh = 1;
            printCenter("Pitch",
                        cOffset + (c * cWidth) + (grid_notes.w /2),
                        rOffset + (r * rHeight) + grid_param.h + tBord/3, X16);
            grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
            break;
        case cvF_MOD: // mod wheel
            grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
            drawCh = 1;
            printCenter("Mod",
            cOffset + (c * cWidth) + (grid_notes.w /2),
            rOffset + (r * rHeight) + grid_param.h + tBord/3, X16);
            grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
            break;
        case cvF_CCs: // CCs (any)
            grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
            drawCh = 1;
            printCenter("CC*",
            cOffset + (c * cWidth) + (grid_notes.w /2),
            rOffset + (r * rHeight) + grid_param.h + tBord/3, X16);
            grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
            break;
        case cvF_CC74: // CCs (74)
            grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
            drawCh = 1;
            printCenter("CC74",
            cOffset + (c * cWidth) + (grid_notes.w /2),
            rOffset + (r * rHeight) + grid_param.h + tBord/3, X16);
            grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
            break;
        case cvF_AT: // Aftertouch (ch/poly)
            grid_param.draw(cOffset + (c * cWidth), rOffset + (r * rHeight), true);
            drawCh = 1;
            printCenter("AfTch",
            cOffset + (c * cWidth) + (grid_notes.w /2),
            rOffset + (r * rHeight) + grid_param.h + tBord/3, X16);
            grid_trans.draw(cOffset + (c * cWidth) + (cWidth/2), rOffset + (r * rHeight), true);
            break;
    }
    tft.setTextColor(txColor);
    return drawCh;
}

void highlightSelect(int c, int r, bool s)
{
    if (c == -1) { return; }  // current route is not on this page
    uint16_t colr;
    if (s == 1)
    {
        colr = RA8875_GREEN;
    } else {
        colr = RA8875_BLACK;
        knobMax = 7; // subselect off
    }
    if (subSelect == 0) // select current route
    {
        tft.drawRect(cOffset + (cWidth * c), rOffset + (rHeight * r), cWidth-1, rHeight-1, colr);
    } else { // sub selection (channel filtering for routing screen)
        
        
        tft.drawRect(cOffset + (cWidth * c) + grid_notes.w,
                     rOffset + (rHeight * r) + grid_param.h,
                     grid_trans.w-1,
                     grid_trans.h-1,
                     colr);
    }
}

void blankSelect(int c, int r)
{
    if (c == -1) { return; }  // current route is not on this page
    tft.drawRect(cOffset + (cWidth * (c - (pgIn * 6))), rOffset + (rHeight * (r - (pgOut * 6))), cWidth-1, rHeight-1, RA8875_BLACK); // clear last green rectangle
}

// Draw grid lines
void drawGLines()
{
    for (int i = 0; i < rows; i++)
    {
        tft.drawLine(0, rOffset + ((i)*rHeight), WIDE, rOffset + ((i)*rHeight), linClr);
    }
    for (int i = 0; i < columns; i++)
    {
        tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), TALL, linClr);
    }
}

// =============================
// Routing Graphics
// =============================

// likely deprecated with BMP skins
void drawPiano(int c, int r) {
    int KeyW = 5;
    int bKeyH = 20;
    int x = cOffset + (cWidth * c);
    int y = rOffset + (rHeight * r);
    
    // draw keybed/border
    tft.fillRect(x, y+30, 99, 29, RA8875_WHITE);
    tft.drawLine(x, y + 29, x + 99, y + 29, RA8875_BLACK);
    
    // draw e/f and b/c gaps
    tft.drawLine(x+KeyW*5, y+30, x+KeyW*5, y+59, RA8875_BLACK);
    tft.drawLine(x+KeyW*12, y+30, x+KeyW*12, y+59, RA8875_BLACK);
    tft.drawLine(x+KeyW*17, y+30, x+KeyW*17, y+59, RA8875_BLACK);
    
    // draw black keys with gaps
    tft.fillRect(x+KeyW, y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW+2, y+50, x+KeyW+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*3), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*3+2, y+50, x+KeyW*3+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*6), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*6+2, y+50, x+KeyW*6+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*8), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*8+2, y+50, x+KeyW*8+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*10), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*10+2, y+50, x+KeyW*10+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*13), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*13+2, y+50, x+KeyW*13+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*15), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*15+2, y+50, x+KeyW*15+2, y+59, RA8875_BLACK);
    tft.fillRect(x+(KeyW*18), y+30, KeyW, bKeyH, RA8875_BLACK);
    tft.drawLine(x+KeyW*18+2, y+50, x+KeyW*18+2, y+59, RA8875_BLACK);
    
}


// =============================
// Flash ins/outs - NOT IMPLEMENTED YET
// =============================

void flashIn(int inp, int state)
{
    if (pgIn * 6 >= inp)              // don't flash if not on correct page
    {
        return;
    }
    if (state != 0)
    {
        // draw input background
        tft.fillRect(cOffset + (cWidth * 6), 0, cWidth - 1, rOffset - 1, insColFlash);
        //(*elapsedIn+inp) = 0;
        inFlag[inp] = 1;
    }
    else
    {
        // draw input background
        tft.fillRect(cOffset + (cWidth * 6), 0, cWidth - 1, rOffset - 1, insColor);
    }
    
    // redraw input name
    tft.setTextColor(txColor);
    tft.setCursor(cOffset + (inp*cWidth) + tCOffset , rOffset - tROffset);
}

// =============================
// BMP function
// =============================

#define BUFFPIXEL 85

void bmpDraw(const char *filename, int x, int y)
{
    File     bmpFile;
    int      bmpWidth, bmpHeight;   // W+H in pixels
    uint8_t  bmpDepth;              // Bit depth (currently must be 24)
    uint32_t bmpImageoffset;        // Start of image data in file
    uint32_t rowSize;               // Not always = bmpWidth; may have padding
    uint8_t  sdbuffer[3 * BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
    uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
    uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    boolean  goodBmp = false;       // Set to true on valid header parse
    boolean  flip    = true;        // BMP is stored bottom-to-top
    int      w, h, row, col, xpos, ypos;
    uint8_t  r, g, b;
    uint32_t pos = 0, startTime = millis();
    uint8_t  lcdidx = 0;
    
    if ((x >= tft.width()) || (y >= tft.height()))
    {
        return;
    }
    
    Serial.println();
    Serial.print(F("Loading image '"));
    Serial.print(filename);
    Serial.println('\'');
    
    // Open requested file on SD card
    if ((bmpFile = SD.open(filename)) == false)
    {
        Serial.println(F("File not found"));
        tft.setCursor(x,y);
        tft.print("BMP NOT FOUND");
        return;
    }
    
    // Parse BMP header
    if (read16(bmpFile) == 0x4D42)  // BMP signature
    {
        Serial.println(F("File size: "));
        Serial.println(read32(bmpFile));
        (void)read32(bmpFile); // Read & ignore creator bytes
        bmpImageoffset = read32(bmpFile); // Start of image data
        Serial.print(F("Image Offset: "));
        Serial.println(bmpImageoffset, DEC);
        
        // Read DIB header
        Serial.print(F("Header size: "));
        Serial.println(read32(bmpFile));
        bmpWidth  = read32(bmpFile);
        bmpHeight = read32(bmpFile);
        
        if (read16(bmpFile) == 1)  // # planes -- must be '1'
        {
            bmpDepth = read16(bmpFile); // bits per pixel
            Serial.print(F("Bit Depth: "));
            Serial.println(bmpDepth);
            if ((bmpDepth == 24) && (read32(bmpFile) == 0))  // 0 = uncompressed
            {
                goodBmp = true; // Supported BMP format -- proceed!
                Serial.print(F("Image size: "));
                Serial.print(bmpWidth);
                Serial.print('x');
                Serial.println(bmpHeight);
                
                // BMP rows are padded (if needed) to 4-byte boundary
                rowSize = (bmpWidth * 3 + 3) & ~3;
                
                // If bmpHeight is negative, image is in top-down order.
                // This is not canon but has been observed in the wild.
                if (bmpHeight < 0)
                {
                    bmpHeight = -bmpHeight;
                    flip      = false;
                }
                
                // Crop area to be loaded
                w = bmpWidth;
                h = bmpHeight;
                if ((x + w - 1) >= tft.width())
                {
                    w = tft.width()  - x;
                }
                if ((y + h - 1) >= tft.height())
                {
                    h = tft.height() - y;
                }
                
                // Set TFT address window to clipped image bounds
                ypos = y;
                for (row = 0; row < h; row++) // For each scanline...
                {
                    // Seek to start of scan line.  It might seem labor-
                    // intensive to be doing this on every line, but this
                    // method covers a lot of gritty details like cropping
                    // and scanline padding.  Also, the seek only takes
                    // place if the file position actually needs to change
                    // (avoids a lot of cluster math in SD library).
                    if (flip) // Bitmap is stored bottom-to-top order (normal BMP)
                    {
                        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                    }
                    else     // Bitmap is stored top-to-bottom
                    {
                        pos = bmpImageoffset + row * rowSize;
                    }
                    
                    if (bmpFile.position() != pos)   // Need seek?
                    {
                        bmpFile.seek(pos);
                        buffidx = sizeof(sdbuffer); // Force buffer reload
                    }
                    xpos = x;
                    for (col = 0; col < w; col++) // For each column...
                    {
                        // Time to read more pixel data?
                        if (buffidx >= sizeof(sdbuffer))   // Indeed
                        {
                            // Push LCD buffer to the display first
                            if (lcdidx > 0)
                            {
                                tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
                                xpos += lcdidx;
                                lcdidx = 0;
                            }
                            
                            bmpFile.read(sdbuffer, sizeof(sdbuffer));
                            buffidx = 0; // Set index to beginning
                        }
                        
                        // Convert pixel from BMP to TFT format
                        b = sdbuffer[buffidx++];
                        g = sdbuffer[buffidx++];
                        r = sdbuffer[buffidx++];
                        lcdbuffer[lcdidx++] = color565(r, g, b);
                        if (lcdidx >= sizeof(lcdbuffer) || (xpos - x + lcdidx) >= w)
                        {
                            tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
                            lcdidx = 0;
                            xpos += lcdidx;
                        }
                    } // end pixel
                    ypos++;
                } // end scanline
                
                // Write any remaining data to LCD
                if (lcdidx > 0)
                {
                    tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
                    xpos += lcdidx;
                }
                
                Serial.print(F("Loaded in "));
                Serial.print(millis() - startTime);
                Serial.println(" ms");
                
            } // end goodBmp
        }
    }
    
    bmpFile.close();
    if (!goodBmp)
    {
        Serial.println(F("BMP format not recognized."));
    }
    
}

// These read 16- and 32-bit types from the SD card file.

uint16_t read16(File& f)
{
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(File& f)
{
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}

uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

// draw GUI element at destX/destY, true = transparent
void guiElem::draw(uint16_t destX, uint16_t destY, bool transp)
{
    if (transp == true)
    {
        tft.setTextColor(tft.Color565(36, 0, 0)); // transparent, (transp) doesn't currently work
        tft.BTE_move(x, y, w, h, destX, destY, 2, 1, transp);
        tft.setTextColor(txColor);
    } else {
        tft.BTE_move(x, y, w, h, destX, destY, 2);
    }
}

// draw keyboard
void drawKeyboard (int x, int y)
{
    tft.clearScreen(RA8875_BLACK); // clear screen
    
    // draw alphanumeric keys
    int kX, kY;
    int kb_count = 0;
    
    for (int r = 0; r < kb_rows; r++) {
        for (int c = 0; c < kb_rowKeys[r]; c++){
            kX = x + kb_rowOffset[r] + (c * (kb_lt.w + kb_Bord));
            kY = y + (r * (kb_lt.h + kb_Bord));
            kb_lt.draw(kX, kY, true);
            printCenter(kb_alphaNum[kb_count][kb_shift],
                        kX + (kb_lt.w/2), kY + (kb_lt.h/2) - (tft.getFontHeight()/2) );
            kb_count++;
        }
    }
    
    // row 4 non-alphanumeric
    kX = x;
    kY = y + ((kb_lt.h + kb_Bord) * 3);
    kb_dk_long.draw(kX, kY, true); // left shift
    printCenter("shift", kX + (kb_dk_long.w/2),
                kY + ((kb_dk_long.h/2) - (tft.getFontHeight()/2)) );
    
    kX += kb_rowOffset[3] + ((kb_lt.w + kb_Bord) * 7);
    kb_dk_long.draw(kX, kY, true); // Right backsp
    printCenter("delete", kX + (kb_dk_long.w/2),
                kY + ((kb_dk_long.h/2) - (tft.getFontHeight()/2)) );
    
    kY = y + ((kb_dk_long.h + kb_Bord) * 4);
    
    // row 5 non-alphanumeric
    kX = x;
    kY = y + ((kb_lt.h + kb_Bord) * 4);
    
    kb_dk_long.draw(kX, kY, true); // left punct
    printCenter("?&123", kX + (kb_dk_long.w/2),
                kY + ((kb_dk_long.h/2) - (tft.getFontHeight()/2)) );
    
    kX += kb_dk_long.w + kb_Bord; // left
    kb_dk.draw(kX, kY, true);
    printCenter("<-", kX + (kb_dk.w/2),
                kY + ((kb_dk.h/2) - (tft.getFontHeight()/2)) );
    
    kX += kb_lt.w + kb_Bord; // right
    kb_dk.draw(kX, kY, true);
    printCenter("->", kX + (kb_dk.w/2),
                kY + ((kb_dk.h/2) - (tft.getFontHeight()/2)) );
    
    kX += kb_lt.w + kb_Bord; // space bar
    kb_sp.draw(kX, kY, true);
    
    kX += kb_sp.w + kb_Bord; // period/comma
    kb_dk.draw(kX, kY, true);
    printCenter(".", kX + (kb_dk.w/2),
                kY + ((kb_dk.h/2) - (tft.getFontHeight()/2)) );
    
    kX += kb_lt.w + kb_Bord; // Right return
    kb_dk_long.draw(kX, kY, true);
    tft.setCursor(kX + (kb_dk_long.w/2), kY + ((kb_dk_long.h + kb_Bord)/2), true);
    printCenter("return", kX + (kb_dk_long.w/2),
                kY + ((kb_dk_long.h/2) - (tft.getFontHeight()/2)) );
}

#endif /* MR_DRAW_h */
