//
//  MR_DRAW.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef MR_DRAW_h
#define MR_DRAW_h

// =============================
// draw globals box
// =============================
void drawBox() {
    
    // Settings/Home
    tft.fillRect(hbOX, hbOY, hbWidth, hbHeight, hbColor);
    
    // inputs page select Background
    tft.fillRect(tbOX, 0, tbWidth, hbHeight, ibColor);
    curX = tbOX + 1;
    curY = 5;
    dPrint("Inputs");
    
    // outputs page select Background
    tft.fillRect(0, hbHeight, hbWidth, tbHeight, obColor);
    curX = 0;
    curY = hbHeight * 1.6;
    dPrint("Outputs");
    
    // Clock/Tempo Box
    tft.fillRect(tbOX, tbOY, tbWidth, tbHeight, tbColor);
}


// =============================
// draw rows and columns
// =============================


void drawColumns() {
    tft.fillRect(cOffset, 0, WIDE - cOffset, rOffset, insColor);
    tft.setTextColor(txColor, insColor);
    
    MRInputPort *input;
    
    for (int i = 0; i < columns; i++) {
        curY = (i*cWidth) + tCOffset;
        curX = tBord;
        if (menu == 0) {  // routing
            tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), TALL, linClr);
            tft.setRotation(3);
            input = router.inputAt((5-i)+(pgIn *6));
            dPrint(input->name);
            tft.setRotation(curRot);
        } else if (menu == 1) { // cv calibrate
            tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), rOffset, linClr);
            if (i == 5-CVcalSelect) {
                //Serial.println("fillrect");
                tft.fillRect( cOffset + (cWidth * CVcalSelect) + 1, 0, cWidth - 2, rOffset, actFieldBg);
            }
            tft.setRotation(3);
            //Serial.println("dprint");
            
            // router could be used to obtain the CV names here
            //input = router.inputAt((5-i)+(pgIn *6));
            
            dPrint("CV");
            dPrint(6-i);
            tft.setRotation(curRot);
        }
    }
}

void drawRows() {
    
    tft.fillRect(0, rOffset, cOffset, TALL - rOffset, outsColor);
    
    MROutputPort *output;
    
    for (int i = 0; i < rows; i++) {
        //tft.textColor(txColor, outsColor);
        //tft.textSetCursor(0, rOffset + (i*rHeight));
        curX = tBord;
        curY = rOffset + (i*rHeight) + tROffset;
        output = router.outputAt(i+(pgOut *6));
        dPrint(output->name);
        tft.drawLine(0, rOffset + ((i)*rHeight), WIDE, rOffset + ((i)*rHeight), linClr);
    }
}

void drawRouting() {
    for (int i = 0; i < rows; i++) {
        for (int c = 0; c < columns; c++) {
            if (routing[c + (pgIn * 6)][i + (pgOut * 6)] == 1) {
                tft.fillRect(cOffset + (cWidth * c), rOffset + (rHeight * i), cWidth, rHeight, routColor);
            } else {
                tft.fillRect(cOffset + (cWidth * c), rOffset + (rHeight * i), cWidth, rHeight, gridColor);
            }
        }
    }
    drawGLines();
}

// Draw grid lines
void drawGLines() {
    for (int i = 0; i < rows; i++) {
        tft.drawLine(0, rOffset + ((i)*rHeight), WIDE, rOffset + ((i)*rHeight), linClr);
    }
    for (int i = 0; i < columns; i++) {
        tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), TALL, linClr);
    }
}

// =============================
// draw backgrounds; grid, inputs and outputs
// =============================

void drawBGs() {
    //tft.fillRect(cOffset, rOffset, WIDE,TALL, gridColor);  // grid bg
    tft.fillRect(cOffset, 0, WIDE-cOffset, rOffset, insColor);  // inputs bg
    tft.fillRect(0, rOffset, cOffset, TALL-rOffset, outsColor);  // outputs bg
}

// =============================
// draw homescreen
// =============================

void drawHomeScreen() {
    drawBox();
    drawBGs();
    drawRows();
    drawColumns();
    drawRouting();
}


// =============================
// Flash ins/outs
// =============================

void flashIn(int inp, int state) {
    if (pgIn * 6 >= inp){             // don't flash if not on correct page
        return;
    }
    if (state != 0){
        // draw input background
        tft.fillRect( cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, insColFlash);
        //(*elapsedIn+inp) = 0;
        inFlag[inp] = 1;
    } else {
        // draw input background
        tft.fillRect( cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, insColor);
    }
    
    // redraw input name
    tft.textColor(txColor, insColor);
    tft.textSetCursor(cOffset + (inp*cWidth) + tCOffset , rOffset - tROffset);
    
}

// =============================
// BMP function
// =============================

#define BUFFPIXEL 85

void bmpDraw(const char *filename, int x, int y) {
    File     bmpFile;
    int      bmpWidth, bmpHeight;   // W+H in pixels
    uint8_t  bmpDepth;              // Bit depth (currently must be 24)
    uint32_t bmpImageoffset;        // Start of image data in file
    uint32_t rowSize;               // Not always = bmpWidth; may have padding
    uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
    uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
    uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
    boolean  goodBmp = false;       // Set to true on valid header parse
    boolean  flip    = true;        // BMP is stored bottom-to-top
    int      w, h, row, col, xpos, ypos;
    uint8_t  r, g, b;
    uint32_t pos = 0, startTime = millis();
    uint8_t  lcdidx = 0;
    
    if((x >= tft.width()) || (y >= tft.height())) return;
    
    Serial.println();
    Serial.print(F("Loading image '"));
    Serial.print(filename);
    Serial.println('\'');
    
    // Open requested file on SD card
    if ((bmpFile = SD.open(filename)) == false) {
        Serial.println(F("File not found"));
        return;
    }
    
    // Parse BMP header
    if(read16(bmpFile) == 0x4D42) { // BMP signature
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
        
        if(read16(bmpFile) == 1) { // # planes -- must be '1'
            bmpDepth = read16(bmpFile); // bits per pixel
            Serial.print(F("Bit Depth: "));
            Serial.println(bmpDepth);
            if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed
                goodBmp = true; // Supported BMP format -- proceed!
                Serial.print(F("Image size: "));
                Serial.print(bmpWidth);
                Serial.print('x');
                Serial.println(bmpHeight);
                
                // BMP rows are padded (if needed) to 4-byte boundary
                rowSize = (bmpWidth * 3 + 3) & ~3;
                
                // If bmpHeight is negative, image is in top-down order.
                // This is not canon but has been observed in the wild.
                if(bmpHeight < 0) {
                    bmpHeight = -bmpHeight;
                    flip      = false;
                }
                
                // Crop area to be loaded
                w = bmpWidth;
                h = bmpHeight;
                if((x+w-1) >= tft.width())  w = tft.width()  - x;
                if((y+h-1) >= tft.height()) h = tft.height() - y;
                
                // Set TFT address window to clipped image bounds
                ypos = y;
                for (row=0; row<h; row++) { // For each scanline...
                    // Seek to start of scan line.  It might seem labor-
                    // intensive to be doing this on every line, but this
                    // method covers a lot of gritty details like cropping
                    // and scanline padding.  Also, the seek only takes
                    // place if the file position actually needs to change
                    // (avoids a lot of cluster math in SD library).
                    if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
                        pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
                    else     // Bitmap is stored top-to-bottom
                        pos = bmpImageoffset + row * rowSize;
                    
                    if (bmpFile.position() != pos) { // Need seek?
                        bmpFile.seek(pos);
                        buffidx = sizeof(sdbuffer); // Force buffer reload
                    }
                    xpos = x;
                    for (col=0; col<w; col++) { // For each column...
                        // Time to read more pixel data?
                        if (buffidx >= sizeof(sdbuffer)) { // Indeed
                            // Push LCD buffer to the display first
                            if(lcdidx > 0) {
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
                        lcdbuffer[lcdidx++] = color565(r,g,b);
                        if (lcdidx >= sizeof(lcdbuffer) || (xpos - x + lcdidx) >= w) {
                            tft.drawPixels(lcdbuffer, lcdidx, xpos, ypos);
                            lcdidx = 0;
                            xpos += lcdidx;
                        }
                    } // end pixel
                    ypos++;
                } // end scanline
                
                // Write any remaining data to LCD
                if(lcdidx > 0) {
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
    if(!goodBmp) Serial.println(F("BMP format not recognized."));
    
}

// These read 16- and 32-bit types from the SD card file.

uint16_t read16(File& f) {
    uint16_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read(); // MSB
    return result;
}

uint32_t read32(File& f) {
    uint32_t result;
    ((uint8_t *)&result)[0] = f.read(); // LSB
    ((uint8_t *)&result)[1] = f.read();
    ((uint8_t *)&result)[2] = f.read();
    ((uint8_t *)&result)[3] = f.read(); // MSB
    return result;
}

uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

#endif /* MR_DRAW_h */
