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

            bg_inputs.draw(201 + (c * 100), 0, false); // background
            
            int p = (pgIn * 10) + c;
            guiElem * icon = &empty;
            
            if (p < 6) {         // pg0, din
                icon = &ic_din;
            } else if (p < 24) { // pg1-2, usb
                icon = &ic_usb;
            } else if (p > 25 && p < 54) { // pg3-5, daw
                icon = &ic_daw;
            }
            
            icon->draw(201 + 18 + (c * 100), 0, true); // icon
            
            // draw names
            tft.setActiveWindow(cOffset + (c * cWidth) + tBord, cOffset + ((c + 1) * cWidth) - tBord + 60, tBord, rOffset - tBord );
            input = router.inputAt(c + (pgIn * 6));
            tft.setCursor(cOffset + (c * cWidth) + tBord, 60 + tBord);
            tft.print(input->name);
            
        } else if (menu == 1) { // cv calibrate screen
            if (c == CVcalSelect)
            {
                curColor = actFieldBg;
            } else {
                curColor = insColor;
            }
            tft.fillRect(cOffset + (cWidth * c) + 3, 0, cWidth - 2, rOffset + 2, curColor);
            
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
        bg_outputs.draw(0, 121 + (r * 60), false); // background

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
        
        icon->draw(0, 121 + (r * 60), true); // icon
        
        // draw names
        tft.setCursor(64 + tBord, rOffset + (r * rHeight) + tROffset);
        output = router.outputAt(r + (pgOut *6));
        tft.print(output->name);
    }
}

void drawRouting() {
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            drawRoute(c, r);
        }
    }
}

void drawRoute(int c, int r)
{
    curRoute = routing[c + (pgIn * 6)][r + (pgOut * 6)]; // store current routing point
    if ((curRoute & B00000111) != 0) {  // draw routed
        grid_routed.draw(201 + (c * 100), 122 + (r * 60), false); // bg

        // draw piano
        if (curRoute & B00000001) {
            grid_notes.draw(201 + (c * 100), 122 + (r * 60) + 26, true);
        }
        // draw parameter
        if (curRoute & B00000010) {
            grid_param.draw(201 + (c * 100), 122 + (r * 60), true);
        }
        // transport
        if (curRoute & B00000100) {
            grid_trans.draw(201 + (c * 100) + 50, 122 + (r * 60), true);
        }
    } else {
        // draw unrouted
        grid_unrouted.draw(201 + (c * 100), 121 + (r * 60), false);

    }
    // draw green rectangle around current selected route
    if ( (curCol - (pgIn * 6) == c) && (curRow - (pgOut * 6)  == r) ) {
        tft.drawRect(cOffset + (cWidth * c) + 2, rOffset + (rHeight * r) + 2, cWidth, rHeight, RA8875_GREEN);
    }
}

void blankSelect()
{
    tft.drawRect(cOffset + (cWidth * (curCol - (pgIn * 6))) + 2, rOffset + (rHeight * (curRow - (pgOut * 6))) + 2, cWidth, rHeight, RA8875_BLACK); // clear last green rectangle
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
// draw backgrounds; grid, inputs and outputs - likeley deprecated
// =============================

void drawBGs()
{
    tft.fillRect(cOffset, 0, WIDE - cOffset, rOffset, insColor); // inputs bg
    tft.fillRect(0, rOffset, cOffset, TALL - rOffset, outsColor); // outputs bg
}

// =============================
// draw homescreen
// =============================

void drawHomeScreen()
{
    drawBox();
    //drawBGs();
    drawRows();
    drawColumns();
    drawRouting();
}

// =============================
// Routing Graphics
// =============================

// likely deprecated with BMP skins
void drawPiano(int c, int r) {
    int KeyW = 5;
    int bKeyH = 20;
    int x = cOffset + (cWidth * c) + 1;
    int y = rOffset + (rHeight * r) + 1;
    
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
// Flash ins/outs
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
        tft.fillRect(cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, insColFlash);
        //(*elapsedIn+inp) = 0;
        inFlag[inp] = 1;
    }
    else
    {
        // draw input background
        tft.fillRect(cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, insColor);
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

//-----------------------
/* This code tested if the bmp bottleneck was the SD card and if drawing from program memory would be faster. The result proved the SD card was not the bottleneck. -eb
void drawArray(uint16_t x, uint16_t y)
{
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0;
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer

  // Crop area to be loaded
  w = in_midi.width;
  h = in_midi.height;
  if((x+w-1) >= WIDE) w = WIDE - x;
  if((y+h-1) >= TALL) h = TALL - y;
    for (int row = 0; row < w; row++)
    {
        for (int column = 0; column < h; column++)
        {
            tft.drawPixel(x + column, y + row, in_midi.pixel_data[(column * w) + row]);
            //Serial.print(row); Serial.print(" ");
        }
        //Serial.println("/");
    }
}
*/
//-----------------------


// draw GUI element at destX/destY, true = transparent
void guiElem::draw(uint16_t destX, uint16_t destY, bool transp)
{
    if (transp = true)
    {
        tft.setTextColor(tft.Color565(36, 0, 0)); // transparent, (transp) doesn't currently work
        tft.BTE_move(x, y, w, h, destX, destY, 2, 1, transp);
        tft.setTextColor(txColor);
    } else {
        tft.BTE_move(x, y, w, h, destX, destY, 2);
    }
}
#endif /* MR_DRAW_h */
