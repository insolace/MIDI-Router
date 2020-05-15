/*

    MIDI Router
    Created by Eric Bateman (eric at timeline85 dot com)
    http://www.midirouter.com

    MR_TOUCH.h - MIDI Router input processing
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

#ifndef MR_TOUCH_h
#define MR_TOUCH_h

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

                //Serial.println("evaltouch");
                evaltouch();
                //Serial.println("done eval");
            }
        }
    }
}

void drawTouchPos()
{
    itoa(touchX, xstr, 10);
    itoa(touchY, ystr, 10);
    //tft.textMode();
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
        drawMenu_Routing();
    }
    else if (menu != 0)
    {
        //tft.fillScreen(NEWCOLOR(touchX / 3, 0, touchY / 2));
        drawMenu_Calibrate();
    }
}


// =================================
// Routing Table
// =================================

void drawMenu_Routing()
{
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

        curCol = (getTouchCol(touchX) - 1) + (pgIn * 6);
        curRow = (getTouchRow(touchY) - 1) + (pgOut * 6);
        curRoute = routing[curCol][curRow];

        if (curRoute == 0)
        {
            routing[curCol][curRow] = 7; // route all
            knobSet(1);
        }
        else
        {
            routing[curCol][curRow] = 0;  // close route
            knobSet(0);
        }

        drawRouting();
        saveEEPROM();
    }
    else if (touchY >= tbOY && touchX >= tbOX)
    {
        // tempo box!
        //drawRows();
        profileInstruments();
    }
    else if (touchY <= tbOY && touchX >= tbOX)
    {
        // input page
        pgIn++;
        pgIn = (pgIn == inPages) ? 0 : pgIn;
        drawColumns();
        drawRouting();
    }
    else if (touchX <= tbOX && touchY >= tbOY)
    {
        // output page
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
    //Serial.println("refmenu rout");
    if (menu == 0)
    {
        drawBox();
        drawBGs();
        drawRows();
        drawColumns();
        drawRouting();
    } else {
        tft.clearScreen(RA8875_BLACK);
    }
}

// =================================
// CV Calibration
// =================================

void refMenu_Calibrate()    // refresh cv calibration display
{
    //Serial.println("refmenu calib");
    drawBox();
    drawColumns();
    tft.fillRect(0, (rOffset + 1), WIDE, (TALL - rOffset - 1), gridColor);
    tft.setCursor(300, rOffset + 25);
    tft.print("CV Calibration");
    drawMenu_Calibrate_udcv();
    oldPosition = (dacNeg[CVcalSelect] * 4);
    router.encoder().write(oldPosition);  // update
    knobMax = 65535; // set knob Max for CV
}

void drawMenu_Calibrate()    // process touch events
{
    //refMenu_Calibrate();
    if (touchY <= hbHeight && touchX <= hbWidth)    // home button returns to routing
    {
        menu = 0;
        knobMax = 8;
        knobMin = 0;
        knobAccelEnable = 0;
        refMenu_Routing();
    }

    if (withinBox(touchX, touchY, menuCV_butDacNeg5_x, menuCV_butDacNeg5_y, menuCV_butDacNeg5_w, menuCV_butDacNeg5_h))
    {
        actField = 1;  // -5v (low) field selected
        oldPosition = (dacNeg[CVcalSelect] * 4);
        router.encoder().write(oldPosition);  // update
        drawMenu_Calibrate_udcv();  // refresh values
        //Serial.println("Neg!");
        setDAC(CVcalSelect, dacNeg[CVcalSelect]);

    }
    else if (withinBox(touchX, touchY, menuCV_butDacPos5_x, menuCV_butDacPos5_y, menuCV_butDacPos5_w, menuCV_butDacPos5_h))
    {
        actField = 2;  // +5v (high) field selected
        oldPosition = (dacPos[CVcalSelect] * 4);
        router.encoder().write(oldPosition);  // update
        drawMenu_Calibrate_udcv();
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
        drawMenu_Calibrate_udcv();

    }
    else if (withinBox(touchX, touchY, tbOX, tbOY, tbWidth, tbHeight))        // green tempo box
    {
        memset(routing, 0, sizeof(routing));   // clear routing table!
    }
}

void drawMenu_Calibrate_udcv()
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

// =================================
// Knobs
// =================================

void readKnob()
{

    // Decode knob turn activity
    if ((millis() - knobTimer) > knobSlowdown)     // slow down the knob updates
    {
        knobTimer = millis();
        newPosition = router.encoder().read();

        if (newPosition != oldPosition)    // filter out duplicate events
        {
            Serial.print("oldPosition: "); Serial.print(oldPosition); Serial.print(" newPosition: "); Serial.println(newPosition);
            if (newPosition < knobMin)   // limit minimum range
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
            else if (newPosition > (knobMax * 4))     // limit maximum range
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
                Serial.print(" kSpeed: "); Serial.print(kSpeed); Serial.print(" #to chg: "); Serial.println(pow(knobSpeedRate, abs(kSpeed)));

                if (kSpeed < 0)
                {
                    newPosition = newPosition - pow(abs(kSpeed), knobSpeedRate);
                }
                else
                {
                    newPosition = newPosition + pow(abs(kSpeed), knobSpeedRate);
                }
            }
            router.encoder().write(newPosition);
            Serial.print("myEnc: "); Serial.println(router.encoder().read());
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
            Serial.print(" newPos: "); Serial.print(newPosition); Serial.print(" knobVal: "); Serial.print(knobVal); Serial.print(" Dir: "); Serial.print(knobDir);
            oldKnobVal = knobVal;  // store for next comparison

            if (menu == 0)
            {
                /*  if (knobVal > 60000) {
                    knobSet(7);
                    } else if (knobVal > 7) {
                    knobZero();
                    }*/
                //knobSet(reOrderR(knobVal));
                routing[curCol][curRow] = reOrderR(knobVal); // routing page, change routing value
                drawRouting();
                Serial.print(" rout: "); Serial.println(routing[curCol][curRow]);

            }
            else if (menu == 1)
            {
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
    drawMenu_Calibrate_udcv();


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

#endif /* MR_TOUCH_h */
