#include "touchscreen.hpp"

#include "main.hpp"
#include "storage.hpp"
#include "MRPorts.h"
#include "midi.hpp"
#include "cvDAC.hpp"
#include "keyboard.hpp"

USING_NAMESPACE_MIDIROUTER

// Initialize graphics
RA8875 tft = RA8875(RA8875_CS, RA8875_RESET);


// =====================================================
// Touchscreen and GUI elements begin here
// =====================================================

// GUI Elements read from BMP on SD card

// Utility
guiElem empty (0, 0, 0, 0);
// backgrounds
guiElem bg_homebox (0, 0, 200, 121);
guiElem bg_inputs (200, 0, 100, 121);
guiElem bg_outputs (0, 121, 201, 60);

// routing grid
guiElem grid_routed (201, 240, 98, 60);
guiElem grid_unrouted (201, 121, 100, 60);
guiElem grid_notes (201, 328, 50, 31);
guiElem grid_param (201, 299, 49, 29);
guiElem grid_trans (252, 299, 48, 26);

// icons
guiElem ic_routing (0, 240, 198, 119);
guiElem ic_calib_back (0, 360, 98, 60);
guiElem ic_calib_clear (101, 360, 98, 60);
guiElem ic_din (0, 181, 62, 60);
guiElem ic_usb (69, 181, 62, 60);
guiElem ic_daw (139, 181, 62, 60);
guiElem ic_eur (217, 181, 57, 55);



// devices
int rows = 6; ///< Number of rows in routing UI
int columns = 6; ///< Number of columns in routing UI

// Menu options
int menu = 0;  ///< which menu are we looking at?  0 = routing, 1 = CV calibration
int actField = 1; ///< which data entry field on the page is active?

boolean rdFlag = 0; ///< flag to redraw screen

// Routing page
int inPages = 6; ///< Number of input pages in routing UI
int outPages = 7; ///< Number of output pages in routing UI

int pgOut = 0; ///< Current output page displayed in routing UI
int pgIn = 0; ///< Current input page displayed in routing UI

// timers to flash inputs
elapsedMillis elapseIn; ///< Timer for flashing inputs
elapsedMillis elapseIn1; ///< Timer for flashing input column 1
elapsedMillis elapseIn2; ///< Timer for flashing input column 2
elapsedMillis elapseIn3; ///< Timer for flashing input column 3
elapsedMillis elapseIn4; ///< Timer for flashing input column 4
elapsedMillis elapseIn5; ///< Timer for flashing input column 5
elapsedMillis elapseIn6; ///< Timer for flashing input column 6

unsigned int flashTime = 1000; ///< delay in milliseconds between activity flashes
int inFlag[5]; ///< Flag to flash an input column


// ****************************************************************************************************
// ***************** CONSTANTS ************************************************************************
// ****************************************************************************************************

// Colors
const uint16_t tbColor     = tft.Color565(0, 150, 0);       // tempo/clock box
const uint16_t hbColor     = tft.Color565(102, 102, 102);   // setup/home button
const uint16_t ibColor     = tft.Color565(0, 0, 150);       // input page box
const uint16_t insColor    = tft.Color565(0, 0, 100);       // inputs box color
const uint16_t insColFlash = tft.Color565(0, 0, 255);       // input flash color
const uint16_t obColor     = tft.Color565(150, 0, 0);       // output page box
const uint16_t outsColor   = tft.Color565(100, 0, 0);       // putputs box color
const uint16_t outColFlash = tft.Color565(255, 0, 0);       // output flash color
const uint16_t gridColor   = tft.Color565(102, 102, 102);   // grid
const uint16_t linClr      = tft.Color565(0, 0, 0);         // lines
const uint16_t txColor     = tft.Color565(182, 182, 170);   // text
const uint16_t routColor   = tft.Color565(255, 255, 255);   // routing
const uint16_t actFieldBg  = tft.Color565(0, 0, 255);       // Active Field color
const uint16_t fieldBg     = tft.Color565(50, 50, 50);      // inactive field color
const uint16_t transp      = tft.Color565(36, 0, 0);         // Transparent key color

// Screen
const int WIDE = 799; ///< Width of touchscreen (0-799)
const int TALL = 479; ///< Height of touchscreen (0-479)

// Rotation, 0 = normal, 2 = 180
const int curRot = 2; ///< Current screen rotation (2 = 180 degrees);



// Font color
uint16_t fColor = RA8875_WHITE; ///< Font Color
uint16_t fBG = 0; ///< Font Backgeround Color

// Font dim
RA8875tsize fSize = X24; ///< Font size X16, X24, X32
RA8875tsize pfSize = X16; ///< Previous font size
int fWidth = 18; ///< Font Width
int fHeight = 25; ///< Font Height
uint16_t curX = 20; ///< Current X coordinate to draw to
uint16_t curY = 20; ///< Current Y coordinate to draw to
int tBord = 10; ///< buffer/border from edge of screen to beginning of text

int arial16CW[95] = {
    4, 4, 8, 9, 9, 16, 12, 4, 5, 5, 6, 9, 4, 5, 4, 4, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 6, 6, 9, 9, 9, 10, 16, 11, 12, 12, 12, 11, 10, 12, 12, 4, 9, 12, 10, 13, 12, 12, 11, 12, 12, 11, 10, 12, 11, 15, 11, 10, 9, 5, 4, 5, 9, 9, 5, 9, 10, 9, 10, 9, 5, 10, 10, 4, 4, 9, 4, 14, 10, 10, 10, 10, 6, 9, 5, 10, 9, 13, 9, 9, 9, 6, 4, 6, 9};

int arial24CW[95] = { ///< Character width for Arial X24 ROM font, starts at ASCII 32 (space)
    7, 7, 11, 13, 13, 18, 17, 6, 8, 8, 9, 14, 7, 8, 7, 7, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 7, 7, 14, 14, 14, 15, 23, 17, 17, 17, 17, 16, 15, 19, 17, 7, 13, 17, 15, 21, 17, 18, 16, 18, 17, 16, 15, 17, 16, 23, 16, 15, 14, 8, 7, 8, 14, 13, 8, 13, 15, 13, 15, 13, 8, 15, 15, 7, 7, 13, 7, 21, 15, 15, 15, 15, 9, 13, 8, 15, 13, 19, 13, 13, 12, 9, 7, 9, 14};

int arial32CW[95] = {
    9, 10, 15, 18, 18, 26, 23, 8, 11, 11, 12, 19, 9, 11, 9, 9, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 10, 10, 19, 19, 19, 20, 31, 23, 22, 23, 23, 21, 20, 25, 22, 8, 17, 23, 20, 27, 22, 24, 20, 24, 22, 21, 20, 22, 21, 31, 21, 20, 20, 11, 9, 11, 19, 18, 11, 18, 19, 18, 19, 18, 11, 19, 19, 10, 10, 18, 10, 28, 19, 19, 19, 19, 13, 18, 11, 19, 17, 25, 18, 17, 16, 12, 9, 12, 19};

// =================================
// Routing menu graphic definitions
// =================================

// Rows
int rOffset = 121;  ///< was 152 - Offset from top of screen to first row
int rHeight = (TALL - rOffset) / rows;  ///< 60 -  Row height
int tROffset = (rHeight / 2) - (fHeight / 2); ///< text vertical offset for rows

// Columns
int cOffset = 201;  ///< was 238 - Offset from left side of screen to first column
int cWidth = (WIDE - cOffset) / columns;  ///< 100 - Column width
int tCOffset = (cWidth / 2) - (fHeight / 5); ///< text horizontal offset for columns

// Tempo Box
float tbWidth = cOffset / 2; ///< Tempo box width
float tbHeight = rOffset / 2; ///< Tempo box height
float  tbOX = (cOffset - tbWidth);    ///< Tempo box origin X
float  tbOY = (rOffset - tbHeight);  ///< Tempo box origin Y
int  tempo = 120; ///< Current tempo - not implemented

// Settings/Home box
float hbWidth = (cOffset - tbWidth); ///< Home box width
float hbHeight = (rOffset - tbHeight); ///< Home box height
int hbOX = 0;    ///< Home box origin X
int hbOY = 0;    ///< Home box origin Y

// CV Calibration menu definitions
int menuCV_butDacNeg5_x = 150,  ///< X coordinate for -5VDC calibration box
    menuCV_butDacNeg5_y = rOffset + 100, ///< Y coordinate for -5V calibration box
    menuCV_butDacNeg5_w = 125,  ///< Width for -5V calibration box
    menuCV_butDacNeg5_h = 50;   ///< Height for -5V calibration box
int menuCV_butDacPos5_x = 460,  ///< X coordinate for +5VDC calibration box
    menuCV_butDacPos5_y = rOffset + 100, ///< Y coordinate for +5VDC calibration box
    menuCV_butDacPos5_w = 125,  ///< Width for +5VDC calibration box
    menuCV_butDacPos5_h = 50;   ///< Height for +5VDC calibration box
int CVcalSelect = 0; ///< Current CV/DAC output selected for calibration


// ============================================================
// UI Routing Display
// ============================================================

float clearRouting = 0; ///< Flag to clear all routings
float pi = 3.141592; ///< Store this in EEPROM to indicate that this isn't the first time we've turned on the MIDI Router. If EEPROM doesn't contain this value, write Zero's to all routing points (EEPROM factory default values are -1)

int curRoute = 0;  ///< Storage for current routing/filter value
int curCol = 0; ///< Current selected column of routing point (0-49)
int curRow = 0; ///< Current selected row of routing point (0-49

int selCol = 0; ///< Current selected displayed column (0-5, -1 = nothing);
int selRow = 0; ///< Current selected displayed row (0-5, -1 = nothing);
bool subSelect = 0; /// < Flag for selecting sub-element


// ============================================================
// Knob values
// ============================================================
long oldPosition = 0; ///< encoder old position
long newPosition = 0; ///< encoder new position
int knobVal = 0; ///< encoder knob value
int oldKnobVal = 0; ///< encoder old knob value
int knobSave = 0; ///< store a knob value for later (sub selection)
bool knobDir = 0;  ///< encoder knob direction 0 = CCW, 1 = CW
bool knobAccelEnable = 0; ///< encoder knob acceleration
unsigned long knobTimer = millis(); ///< encoder knob timer
unsigned long knobSlowdown = 2;  ///< wait this many ms before checking the knob value
int knobSpeedup = 3; ///< threshold for difference between old and new value to cause a speed up
float knobSpeedRate = 2.8; ///< factor (exponent) to speed up by
int knobMin = 0; ///< encoder knob min value
int knobMax = 7; ///< encoder knob max value

// ============================================================
// TFT GRAPHICS
// ============================================================

void initDisplay()
{
    // Initialise the TFT display
    tft.begin(RA8875_800x480);
    
    if (tft.errorCode() != 0)  {
        Serial.println("RA8875 Not Found!");
        delay(100);
    } else {
    Serial.println("Found RA8875");
    }
    tft.backlight(0);     // backlight off, avoid bright flashes at startup
    
    // Set screen brightness
    tft.brightness(255);
    
    // Rotation
    tft.setRotation(curRot);
    
    // External fonts
    tft.setExternalFontRom(ER3304_1, ASCII);
    //tft.setFont(INTFONT);
    tft.setFont(EXTFONT);
    tft.setExtFontFamily(ARIAL);
    fontSize(X24);
    tft.setFontScale(0);
    tft.setTextColor(txColor);
    tft.clearScreen(RA8875_BLACK); // clear screen
}

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
    #ifdef SDCARD_SUPPORT
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
    #endif // #ifdef SDCARD_SUPPORT
}

// These read 16- and 32-bit types from the SD card file.

#ifdef SDCARD_SUPPORT
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
#endif

uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
{
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

/// Constructor
/// @param a x origin of element
/// @param b y origin of element
/// @param c width of element
/// @param d height of element
guiElem::guiElem(uint16_t a, uint16_t b, uint16_t c, uint16_t d)
{
    x = a; y = b; w = c; h = d;
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



// HANDLE TEXT


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
    for (uint16_t i = 0; i < s.length(); i++)
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
    unsigned int sLen = 0;
    for (unsigned int i = 0; i < s.length(); i++)
    {
        sLen += wArray[(char(s[i])-32)];
    }
    return sLen;
}


// ************************************************************************************************************************************
// ************************************************************************************************************************************
// CAPACITIVE TOUCH INTERFACE
// ************************************************************************************************************************************
// ************************************************************************************************************************************


// touch
GSL1680 TS = GSL1680(); ///< Touch screen driver

char ystr[16];///< Array to convert touchY from a string for debugging
char xstr[16];///< Array to convert touchX from a string for debugging
long touchX = 0;///< X coordinate of last touchpoint
long touchY = 0;///< Y coordinate of last touchpoint
long lastPress = 0;///< Timer for the last time we received a touch interrupt. Filters out duplicate events.
long newX = 0;///< X coordinate of newly received touchpoint
long newY = 0;///< Y coordinate of newly received touchpoint
int difX = 0;///< The difference between the last touch X coordinate and the one just received. Filters out duplicate events.
int difY = 0;///< The difference between the last touch Y coordinate and the one just received. Filters out duplicate events.

unsigned long touchShort = 300;     ///< (ms) must touch this long to trigger
int tMargin = 5;       ///< pixel margin to filter out duplicate triggers for a single touch

long fingers, curFing, x, y;


void touchIO()
{
    if (digitalRead(INTRPT) == HIGH && ((millis() - lastPress) > touchShort))
    {
        lastPress = millis();
        //drawTouchPos();
        fingers = TS.dataread();
        curFing = TS.readFingerID(0);  // touchscreen can read up to 10 fingers,
        if (1)
        {
            switch (curRot)
            {
                case 2:
                    newX = WIDE - TS.readFingerX(0);
                    newY = TALL - TS.readFingerY(0);
                    break;
                default:
                    newX = TS.readFingerX(0);
                    newY = TS.readFingerY(0);
                    break;
            }
            difX = newX - touchX;
            difY = newY - touchY;
            if ((abs(difX) > tMargin || abs(difY) > tMargin))
            {
                touchX = newX;
                touchY = newY;

                evaltouch();
            }
        }
    }
}

void drawTouchPos() // debug, show current touch position
{
    itoa(touchX, xstr, 10);
    itoa(touchY, ystr, 10);
    tft.setCursor(20, 40);
    tft.print(" X:");
    tft.print(xstr);
    tft.print(" Y:");
    tft.print(ystr);
}

void evaltouch()
{
    if (menu == 0)
    {
        update_Routing();
    }
    else if (menu != 0)
    {
        update_Calibrate();
    }
}


// =================================
// Update routing table UI
// =================================

void update_Routing()
{
    subSelect = 0; // remove sub selection
    drawRoute(selCol, selRow); // update previous routing point (clean up sub selection)
    highlightSelect(selCol, selRow, 0); // clear last selected routing point highlight
    //Serial.println("dm rout");
    if (touchY <= rOffset && touchX >= cOffset)
    {
        // input selected
    }
    else if (touchY >= rOffset && touchX <= cOffset)
    {
        // output selected
    }
    else if (touchY >= rOffset && touchX >= cOffset)
    {
        // routing grid selected
        selCol = getTouchCol(touchX) - 1;
        selRow = getTouchRow(touchY) - 1;
        curCol = selCol + (pgIn * 6);
        curRow = selRow + (pgOut * 6);
        curRoute = routing[curCol][curRow];

        if (curRoute == 0)
        {
            if (pgOut < 6)
            {
                routing[curCol][curRow] = 7; // route all (MIDI)
            } else {
                routing[curCol][curRow] = 1; // CV start with noteon
            }
            knobSet(1);
        }
        else
        {
            routing[curCol][curRow] = 0;  // close route
            knobSet(0);
        }
        drawRoute(selCol, selRow);
        saveEEPROM();
    }
    else if (touchY >= tbOY && touchX >= tbOX)
    {
        // tempo box!
        profileInstruments();
    }
    else if (touchY <= tbOY && touchX >= tbOX)
    {
        // input page
        //selCol = -1; // clear selected col/row
        //selRow = -1;
        
        pgIn++;
        pgIn = (pgIn == inPages) ? 0 : pgIn;
        drawColumns();
        drawRouting();
    }
    else if (touchX <= tbOX && touchY >= tbOY)
    {
        // output page
        //selCol = -1; // clear selected col/row
        //selRow = -1;
        
        pgOut++;
        pgOut = (pgOut == outPages) ? 0 : pgOut;
        drawRows();
        drawRouting();
    }
    else
    {
        // setup/home box, go to CV Calibration
        menu = 1;
        knobAccelEnable = 1;
        refMenu_Calibrate();
    }
}

void refMenu_Routing()    // refresh routing display
{
    tft.backlight(0); // clean transitions
    if (menu == 0)
    {
        drawBox();
        drawRows();
        drawColumns();
        drawRouting();
    } else {
        tft.clearScreen(RA8875_BLACK);
    }
    tft.backlight(1); // clean transitions
}

// =================================
// CV Calibration
// =================================

void refMenu_Calibrate()    // refresh cv calibration display
{
    tft.backlight(0); // clean transitions
    drawBox();
    drawColumns();
    //highlightSelect(selCol, selRow, 0); // clear last selected routing point
    tft.fillRect(0, (rOffset + 3), WIDE, (TALL - rOffset - 1), gridColor); // blank out routing grid
    tft.setCursor(300, rOffset + 25);
    tft.print("CV Calibration");
    update_Cal_Values();
    tft.backlight(1); // clean transitions
    
    oldPosition = (dacNeg[CVcalSelect] * 4);
    router.encoder().write(oldPosition);  // update
    knobMax = 65535; // set knob Max for CV
}

void update_Calibrate()    // process touch events for calibration screen
{
    if (touchY <= hbHeight && touchX <= hbWidth)    // home button returns to routing
    {
        menu = 0;
        knobMax = 7;
        knobMin = 0;
        knobAccelEnable = 0;
        refMenu_Routing();
    }

    if (withinBox(touchX, touchY, menuCV_butDacNeg5_x, menuCV_butDacNeg5_y, menuCV_butDacNeg5_w, menuCV_butDacNeg5_h))
    {
        actField = 1;  // -5v (low) field selected
        oldPosition = (dacNeg[CVcalSelect] * 4);
        router.encoder().write(oldPosition);  // update
        update_Cal_Values();  // refresh values
        //Serial.println("Neg!");
        setDAC(CVcalSelect, dacNeg[CVcalSelect]);

    }
    else if (withinBox(touchX, touchY, menuCV_butDacPos5_x, menuCV_butDacPos5_y, menuCV_butDacPos5_w, menuCV_butDacPos5_h))
    {
        actField = 2;  // +5v (high) field selected
        oldPosition = (dacPos[CVcalSelect] * 4);
        router.encoder().write(oldPosition);  // update
        update_Cal_Values();
        //Serial.println("Pos!");
        setDAC(CVcalSelect, dacPos[CVcalSelect]);

    }
    else if (withinBox(touchX, touchY, cOffset, 0, WIDE - cOffset, rOffset))       // select CV
    {
        CVcalSelect = abs((touchX - cOffset) / ((WIDE - cOffset) / 6));
        //Serial.print("cvSel: "); Serial.println(CVcalSelect);
        actField = 1; // select neg field
        setDAC(CVcalSelect, dacNeg[CVcalSelect]);
        if (CVcalSelect > 3)
        {
            knobMax = 4096;
        }
        else
        {
            knobMax = 65535;
        }
        oldPosition = (dacNeg[CVcalSelect] * 4);
        router.encoder().write(oldPosition);  // update
        drawColumns();
        update_Cal_Values();

    }
    else if (withinBox(touchX, touchY, tbOX, tbOY, tbWidth, tbHeight))        // green tempo box
    {
        memset(routing, 0, sizeof(routing));   // clear routing table!
    }
}

void update_Cal_Values()
{

    if (actField == 1)    //neg5
    {
        negCol = actFieldBg;
        posCol = fieldBg;

    }
    else      //pos5
    {
        posCol = actFieldBg;
        negCol = fieldBg;

    }
    tft.fillRect(menuCV_butDacNeg5_x, menuCV_butDacNeg5_y, menuCV_butDacNeg5_w, menuCV_butDacNeg5_h, negCol);
    tft.fillRect(menuCV_butDacPos5_x, menuCV_butDacPos5_y, menuCV_butDacPos5_w, menuCV_butDacPos5_h, posCol);
    tft.setCursor(menuCV_butDacNeg5_x + 5, menuCV_butDacNeg5_y + 5);
    tft.print(dacNeg[CVcalSelect]);
    
    tft.setCursor(menuCV_butDacPos5_x + 5, menuCV_butDacPos5_y + 5);
    tft.print(dacPos[CVcalSelect]);
}



// for any given X value of the touch, return the column of our grid
int getTouchCol(long x)
{
    if (x > (WIDE - 3) - ((WIDE - cOffset) / columns))
    {
        return (6);
    }
    else if (x > (WIDE - 3) - (((WIDE - cOffset) / columns)) * 2)
    {
        return (5);
    }
    else if (x > (WIDE - 3) - (((WIDE - cOffset) / columns)) * 3)
    {
        return (4);
    }
    else if (x > (WIDE - 3) - (((WIDE - cOffset) / columns)) * 4)
    {
        return (3);
    }
    else if (x > (WIDE - 3) - (((WIDE - cOffset) / columns)) * 5)
    {
        return (2);
    }
    else
    {
        return (1);
    }
}

// for any given Y value of the touch, return the row of our grid
int getTouchRow(long y)
{
    if (y > (TALL + 3) - ((TALL - rOffset) / rows))
    {
        return (6);
    }
    else if (y > (TALL + 3) - (((TALL - rOffset) / rows)) * 2)
    {
        return (5);
    }
    else if (y > (TALL + 3) - (((TALL - rOffset) / rows)) * 3)
    {
        return (4);
    }
    else if (y > (TALL + 3) - (((TALL - rOffset) / rows)) * 4)
    {
        return (3);
    }
    else if (y > (TALL + 3) - (((TALL - rOffset) / rows)) * 5)
    {
        return (2);
    }
    else
    {
        return (1);
    }
}


// ************************************************************************************************************************************
// ************************************************************************************************************************************
// ENCODER
// ************************************************************************************************************************************
// ************************************************************************************************************************************





// =================================
// Knobs
// =================================

void readKnob() // TODO: 4 clicks per knob, plus acceleration, = slippage. This whole routine may need to be redone.
{

    // Decode knob turn activity
    if ((millis() - knobTimer) > knobSlowdown)     // slow down the knob updates
    {
        knobTimer = millis();
        newPosition = router.encoder().read();

        if (newPosition != oldPosition)    // filter out duplicate events
        {
//            Serial.print("oldPosition: "); Serial.print(oldPosition); Serial.print(" newPosition: "); Serial.println(newPosition);
            if (newPosition < (knobMin * 4))   // limit minimum range
            {
                if (menu == 1)    // CV cal menu = stop at zero
                {
                    knobZero();
                }
                else if (menu == 0)     // routing menu = comes back around to max
                {
                    knobFull();
                }
            }
            else if (newPosition >= ((knobMax+1) * 4))     // limit maximum range
            {
                if (menu == 1)   // CV cal menu = stop at max
                {
                    knobFull();
                }
                else if (menu == 0)     // routing menu = comes back around to min
                {
                    knobZero();
                }
            }
            // acceleration
            float kSpeed = newPosition - oldPosition; // speed up value change if knob is turning fast
            if (((kSpeed < 30) && (abs(kSpeed) > knobSpeedup)) && knobAccelEnable == 1)         // Counter clockwise rotation spits out occasional +80 errors due to interrupt optimization.
            {
                // Filter out anything greater than 30 (dirty fix)
                //Serial.print(" kSpeed: "); Serial.print(kSpeed); Serial.print(" #to chg: "); Serial.println(pow(knobSpeedRate, abs(kSpeed)));

                if (kSpeed < 0)
                {
                    newPosition = newPosition - pow(abs(kSpeed), knobSpeedRate);
                }
                else
                {
                    newPosition = newPosition + pow(abs(kSpeed), knobSpeedRate);
                }
            }
            //router.encoder().write(newPosition);
            //Serial.print("myEnc: "); Serial.println(router.encoder().read());
        }
        oldPosition = newPosition;
        knobVal = newPosition / 4; // four pulses per encoder click, scale down so 1 knob val = 1 click
        if (oldKnobVal != knobVal)   // is the scaled value new?
        {
            // update knob value and DO SOMETHING
            if (oldKnobVal < knobVal)
            {
                knobDir = 1; // CW
            }
            else
            {
                knobDir = 0; // CCW
            }
//            Serial.print(" newPos: "); Serial.print(newPosition);
//            Serial.print(" knobVal: "); Serial.println(knobVal);
//            Serial.print(" oldKnobVal: "); Serial.println(oldKnobVal);
//            Serial.print(" Dir: "); Serial.println(knobDir);
            oldKnobVal = knobVal;  // store for next comparison

            if (menu == 0) // Routing grid
            {
                if (subSelect == 0) // changing routing filter state
                {
                    int curChan = routing[curCol][curRow] & B11111000;
                    int curFilt = 0;
                    if (pgOut < 6) // reorder for MIDI
                    {
                        curFilt = reOrderR(knobVal);
                    } else {
                        curFilt = knobVal & B00000111;
                    }
                    
                    int newRouteVal = curChan | curFilt;
                    routing[curCol][curRow] = newRouteVal; // routing page, change routing value
                    drawRoute(curCol - (pgIn * 6), curRow - (pgOut * 6));
                } else { // change channel filters
                    int curChan = knobVal << 3;
                    int curFilt = routing[curCol][curRow] & B00000111;
                    int newRouteVal = curChan | curFilt;
                    routing[curCol][curRow] = newRouteVal;
                    drawRoute(selCol, selRow);
                }
                saveEEPROM();
            } else if (menu == 1){ // CV
                knob_calCV();  // calibrate CV
            }
        }
    }

    // encoder switch
    router.encPush().update();
    if (router.encPush().fell())
    {
        Serial.println("push");  // knob pushed, do something

        if (menu == 1)   // CV calibration
        {
            if (knobVal == 0)   // change to max value
            {

                oldPosition = knobMax * 4;
                newPosition = oldPosition;
                router.encoder().write(newPosition);
            }
            else      // change to min value
            {
                oldPosition = knobMin * 4;
                newPosition = oldPosition;
                router.encoder().write(newPosition);
            }
            knobVal = newPosition / 4;
            knob_calCV();
        } else if (menu == 0) {  // Routing screen
            curRoute = routing[selCol + (pgIn * 6)][selRow + (pgOut * 6)]; // gather current routing value

            if ( (curRoute & B00000111) == 4 ||
                (curRoute & B00000111) == 0) // Unrouted or just transport
            {
                subSelect = 0;
                drawRoute(selCol, selRow); // redraw route
                return;
            }
            highlightSelect(selCol, selRow, 0); // clear previous selection
            subSelect = !subSelect;
            drawRoute(selCol, selRow); // redraw route
            if (subSelect == 1)
            {
                knobSave = knobVal; // store
                knobSet(routing[curCol][curRow] >> 3); // bits 3-7 are our filtered channel val, 0 = all, 1-16 = binary 0-15
                
                knobMax = 16; //
                //Serial.print("fCH:"); Serial.println(knobVal);
            } else {
                knobSet(knobSave); // restore
                knobMax = 7; // 8 different routed filter states
            }
                
        }

    }
}

void knobZero()
{
    newPosition = knobMin;
    oldPosition = knobMin;
    router.encoder().write(0);
}

void knobFull()
{
    newPosition = knobMax * 4;
    oldPosition = newPosition;
    router.encoder().write(newPosition);
}

void knobSet(int v)
{
    newPosition = (v) * 4;
    oldPosition = newPosition;
    router.encoder().write(newPosition);
    knobVal = v;
    oldKnobVal = knobVal;
}
// =================================
// Knob functions
// =================================

void knob_calCV()
{
    //Serial.print("knobVal: "); Serial.println(knobVal);
    if (actField == 1)   // neg5
    {
        //Serial.print("neg: "); Serial.println(dacNeg[CVcalSelect]);
        dacNeg[CVcalSelect] = knobVal;
        setDAC(CVcalSelect, dacNeg[CVcalSelect]);
    }
    else if (actField == 2)
    {
        //Serial.print("pos: "); Serial.println(dacPos[CVcalSelect]);
        dacPos[CVcalSelect] = knobVal;
        setDAC(CVcalSelect, dacPos[CVcalSelect]);
    }
    saveEEPROM();
    update_Cal_Values();


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

// =================================
// Button functions
// =================================

boolean withinBox(int x, int y, int bx, int by, int bw, int bh)
{
    if (x >= bx && x <= (bx + bw))   // test if X is greater than the box origin x, and less than bx+width
    {
        if (y >= by && y <= (by + bh))
        {
            return 1;
        }
    }
    return 0;
}
