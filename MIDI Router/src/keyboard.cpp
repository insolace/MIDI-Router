
#include "touchscreen.hpp"


// KEYBOARD UI - still in development, currently not implemented

// keyboard
guiElem kb_dk_long (522, 361, 116, 56); // dark long key
guiElem kb_dk (641, 361, 76, 56);       // dark key
guiElem kb_lt (720, 361, 76, 56);       // light key
guiElem kb_hl (720, 421, 76, 56);       // highlighted key
guiElem kb_sp (403, 421, 314, 56);      // space bar


int kb_rows = 4;
int kb_Bord = 3;
int kb_x = 6, kb_y = 178;
int kb_rowOffset[4] = {0, 0, 40, 119};
int kb_rowKeys[4] = {10, 10, 9, 7};

enum KBshift {kCaps=0, kLower, kSym}; // 0, 1, 2
int kb_shift = 0;
char kb_alphaNum[36][3] = {
    {'1','1','!'},
    {'2','2','@'},
    {'3','3','#'},
    {'4','4','$'},
    {'5','5','%'},
    {'6','6','^'},
    {'7','7','&'},
    {'8','8','*'},
    {'9','9','('},
    {'0','0',')'},
    {'Q','q','`'},
    {'W','w','~'},
    {'E','e',' '},
    {'R','r',' '},
    {'T','t','-'},
    {'Y','y','_'},
    {'U','u','='},
    {'I','i','+'},
    {'O','o','|'},
    {'P','p','\\'},
    {'A','a',' '},
    {'S','s',' '},
    {'D','d','{'},
    {'F','f','}'},
    {'G','g',' '},
    {'H','h',';'},
    {'J','j',':'},
    {'K','k','\''},
    {'L','l','"'},
    {'Z','z','<'},
    {'X','x','>'},
    {'C','c','['},
    {'V','v',']'},
    {'B','b',' '},
    {'N','n',','},
    {'M','m','?'}
};


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
