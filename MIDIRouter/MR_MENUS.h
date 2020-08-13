/*

    MIDI Router
    Created by Eric Bateman (eric at timeline85 dot com)
    http://www.midirouter.com

    MR_MENUS.h - MIDI Router input processing
    Source code written by Eric Bateman
    Copyright © 2020 Eric Bateman
 
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

#ifndef MR_MENUS_h
#define MR_MENUS_h

// ===========================================
// TOUCHSCREEN KEYBOARD
// ===========================================

Menu<44> keyboard;

int kX, kY;
int kb_count = 0;

for (int r = 0; r < kb_rows; r++) {
    for (int c = 0; c < kb_rowKeys[r]; c++){
        kX = kb_x + kb_rowOffset[r] + (c * (kb_lt.w + kb_Bord));
        kY = kb_y + (r * (kb_lt.h + kb_Bord));
        keyboard.defElem(kb_count, &kb_lt, kX, kY, true); // element index, address of graphic to draw, x, y, transparent
        keyboard.label[kb_count] = kb_alphaNum[kb_count][kb_shift];
        kb_count++;
    }
}

// row 4 non-alphanumeric
kX = kb_x;
kY = kb_y + ((kb_lt.h + kb_Bord) * 3);

keyboard.defElem(kb_count, &kb_dk_long, kX, kY, true);
keyboard.label[kb_count] = "shift";
kb_count++;

kX += kb_rowOffset[3] + ((kb_lt.w + kb_Bord) * 7);

keyboard.defElem(kb_count, &kb_dk_long, kX, kY, true);
keyboard.label[kb_count] = "delete";
kb_count++;

kY = kb_y + ((kb_dk_long.h + kb_Bord) * 4);

// row 5 non-alphanumeric
kX = kb_x;
kY = kb_y + ((kb_lt.h + kb_Bord) * 4);

keyboard.defElem(kb_count, &kb_dk_long, kX, kY, true);
keyboard.label[kb_count] = "?&123";
kb_count++;

kX += kb_dk_long.w + kb_Bord; // left

keyboard.defElem(kb_count, &kb_dk, kX, kY, true);
keyboard.label[kb_count] = "<-";
kb_count++;

kX += kb_lt.w + kb_Bord; // right

keyboard.defElem(kb_count, &kb_dk, kX, kY, true);
keyboard.label[kb_count] = "->";
kb_count++;

kX += kb_lt.w + kb_Bord; // space bar

keyboard.defElem(kb_count, &kb_sp, kX, kY, true);
kb_count++;

kX += kb_sp.w + kb_Bord; // period/comma

keyboard.defElem(kb_count, &kb_dk, kX, kY, true);
keyboard.label[kb_count] = ".";
kb_count++;

kX += kb_lt.w + kb_Bord; // Right return

keyboard.defElem(kb_count, &kb_dk_long, kX, kY, true);
keyboard.label[kb_count] = "return";
kb_count++;

#endif // MR_MENUS_RELEASE
