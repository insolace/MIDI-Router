void touchIO() {
  if (digitalRead(INTRPT) == HIGH) {
    //drawTouchPos();   
    fingers = TS.dataread();
    curFing = TS.readFingerID(0);  // touchscreen can read up to 10 fingers, 
    if (1) {
      switch (curRot) {
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
      if ( ( abs(difX) > tMargin || abs(difY) > tMargin) && ( (millis() - lastPress) > touchShort) ) {
        touchX = newX;
        touchY = newY;
        lastPress = millis();
        //Serial.println("evaltouch");
        evaltouch();    
        //Serial.println("done eval");        
      }
    }
  }
}

void drawTouchPos() {
  itoa(touchX, xstr, 10);
  itoa(touchY, ystr, 10);  
  tft.textMode();
  tft.textSetCursor(20,40);  
  tft.textWrite(" X:");
  tft.textWrite(xstr);
  tft.textWrite(" Y:");
  tft.textWrite(ystr);  
}

void evaltouch() {

  if (menu == 0) {
    drawMenu_Routing();
  } else if (menu != 0) {
    //tft.fillScreen(newColor(touchX / 3, 0, touchY / 2));  
    drawMenu_Calibrate();
  }
}


// =================================
// Routing Table
// =================================

void drawMenu_Routing() {
  //Serial.println("dm rout");
  if (touchY <= rOffset && touchX >= cOffset) {
    // input selected
  } else if (touchY >= rOffset && touchX <= cOffset) {
    // output selected
  } else if (touchY >= rOffset && touchX >= cOffset) {
    // routing grid selected 
    curGrid = routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)];
    //Serial.print("curgrid: "); Serial.println(curGrid);
    if (curGrid == 1){
      routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)] = 0;      
    } else {
      routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)] = 1;
    }
    drawRouting();
    saveEEPROM();
  } else if (touchY >= tbOY && touchX >= tbOX) {
    // tempo box!
    drawRows();
  } else if (touchY <= tbOY && touchX >= tbOX) {
    // input page
    pgIn++;
    pgIn = (pgIn == inPages) ? 0 : pgIn; 
    drawColumns(); 
    drawRouting();
  } else if (touchX <= tbOX && touchY >= tbOY) {
    // output page
    pgOut++;
    pgOut = (pgOut == outPages) ? 0 : pgOut; 
    drawRows();
    drawRouting();
  } else {
    // setup/home box, go to CV Calibration
    menu = 1;
    refMenu_Calibrate();
  }  
}

void refMenu_Routing() {  // refresh routing display
  //Serial.println("refmenu rout");
  if (menu == 0) {
    drawBox();
    drawBGs();
    drawRows();
    drawColumns();
    drawRouting(); 
  } else {
    tft.fillScreen(RA8875_BLACK);
  }
}

// =================================
// CV Calibration
// =================================

void refMenu_Calibrate() {  // refresh cv calibration display
  //Serial.println("refmenu calib");
  drawBox(); 
  drawColumns();
  tft.fillRect(0,(rOffset+1),WIDE,(TALL-rOffset-1), newColor(gridColor));
  curX = 300;
  curY = rOffset + 25;
  dPrint("CV Calibration");
  drawMenu_Calibrate_udcv();
  oldPosition = (dacNeg[CVcalSelect] * 4);
  myEnc.write(oldPosition);  // update  
  knobMax = 65535; // set knob Max for CV
}

void drawMenu_Calibrate() {  // process touch events
  //refMenu_Calibrate();
  if (touchY <= hbHeight && touchX <= hbWidth) {  // home button returns to routing
    menu = 0;
    refMenu_Routing();
  }

  if ( withinBox(touchX, touchY, menuCV_butDacNeg5_x, menuCV_butDacNeg5_y, menuCV_butDacNeg5_w, menuCV_butDacNeg5_h) ) {
    actField = 1;  // -5 field selected
    oldPosition = (dacNeg[CVcalSelect] * 4);
    myEnc.write(oldPosition);  // update
    drawMenu_Calibrate_udcv();  // refresh values
    //Serial.println("Neg!");
    setDAC(CVcalSelect, dacNeg[CVcalSelect]);

  } else if ( withinBox(touchX, touchY, menuCV_butDacPos5_x, menuCV_butDacPos5_y, menuCV_butDacPos5_w, menuCV_butDacPos5_h) ) {
    actField = 2;
    oldPosition = (dacPos[CVcalSelect] * 4);
    myEnc.write(oldPosition);  // update
    drawMenu_Calibrate_udcv();
    //Serial.println("Pos!");
    setDAC(CVcalSelect, dacPos[CVcalSelect]);

  } else if ( withinBox(touchX, touchY, cOffset, 0, WIDE-cOffset, rOffset) ) {  // select CV
    CVcalSelect = abs((touchX - cOffset) / ((WIDE-cOffset)/6) );
    //Serial.print("cvSel: "); Serial.println(CVcalSelect);
    actField = 1; // select neg field
    setDAC(CVcalSelect, dacNeg[CVcalSelect]);
    if (CVcalSelect > 3) {
      knobMax = 4096;
    } else {
      knobMax = 65535;
    }
    oldPosition = (dacNeg[CVcalSelect] * 4);
    myEnc.write(oldPosition);  // update
    drawColumns();
    drawMenu_Calibrate_udcv();
    
  }
}

void drawMenu_Calibrate_udcv() {

  if (actField == 1) {  //neg5
    negCol = newColor(actFieldBg);
    posCol = newColor(fieldBg);

  } else {  //pos5
    posCol = newColor(actFieldBg);
    negCol = newColor(fieldBg);   

  }
  tft.fillRect(menuCV_butDacNeg5_x, menuCV_butDacNeg5_y, menuCV_butDacNeg5_w, menuCV_butDacNeg5_h, negCol);
  tft.fillRect(menuCV_butDacPos5_x, menuCV_butDacPos5_y, menuCV_butDacPos5_w, menuCV_butDacPos5_h, posCol);  
  curX = menuCV_butDacNeg5_x + 5;
  curY = menuCV_butDacNeg5_y + 5; 
  dPrint(dacNeg[CVcalSelect]);
  
  curX = menuCV_butDacPos5_x + 5;
  curY = menuCV_butDacPos5_y + 5; 
  dPrint(dacPos[CVcalSelect]);  


}

// =================================
// Knobs
// =================================

void readKnob() {

  // Decode knob turn activity
  
  if ( (millis() - knobTimer) > knobSlowdown) {  // slow down the knob updates
    knobTimer = millis();
    long newPosition = myEnc.read();
 
    if (newPosition != oldPosition) {  // filter out duplicate events
      if (newPosition < knobMin) { // limit minimum range
        newPosition = knobMin; 
        oldPosition = knobMin;
        myEnc.write(0);
      } else if (newPosition > (knobMax * 4)) { // limit maximum range
        newPosition = knobMax * 4; 
        oldPosition = knobMax * 4;
        myEnc.write(knobMax * 4);
      }

      int kSpeed = newPosition-oldPosition;  // speed up value change if knob is turning fast
      if (abs(kSpeed) > knobSpeedup) {   
        newPosition = newPosition + (knobSpeedRate * kSpeed * (round(knobMax / 1000)+1)); 
        myEnc.write(newPosition);
      }

      oldPosition = newPosition;
      knobVal = newPosition / 4; // four pulses per encoder click, scale down so 1 knob val = 1 click
      if (oldKnobVal != knobVal) { // is the scaled value new?
        // update knob value and DO SOMETHING
        oldKnobVal = knobVal;  // store for next comparison

        if (menu == 0) {
          menu = 0; // do nothing on routing page
        } else if (menu == 1) {
          knob_calCV();  // calibrate CV
        }
      }

    }
  }

  // encoder switch
  encPush.update();
  if ( encPush.fell() ) {
    Serial.println("push");  // knob pushed, do something
    if (knobVal > (knobMax/2) || knobVal == knobMin) { // change to max value
        myEnc.write(knobMax * 4);
    } else if (knobVal < (knobMax/2) || knobVal == knobMax) {  // change to min value
        myEnc.write(knobMin * 4);
    }
    //Serial.println(myEnc.read());  // knob pushed, do something
  }
}

// =================================
// Knob functions
// =================================

void knob_calCV() {
  //Serial.print("knobVal: "); Serial.println(knobVal);
  if (actField == 1) { // neg5
    //Serial.print("neg: "); Serial.println(dacNeg[CVcalSelect]);
    dacNeg[CVcalSelect] = knobVal;
    setDAC(CVcalSelect, dacNeg[CVcalSelect]);
  } else if (actField == 2) {
    //Serial.print("pos: "); Serial.println(dacPos[CVcalSelect]);
    dacPos[CVcalSelect] = knobVal;  
    setDAC(CVcalSelect, dacPos[CVcalSelect]);
  }
  saveEEPROM();
  drawMenu_Calibrate_udcv();
  
  //cveeKnobOffset = (knobVal - 128) * 10;
  //Serial.println(cveeKnobOffset);   
  //menu = cveeKnobOffset;


  
  //tft.PWM1out(knobVal);  // dim screen 
  //setDAC(4, knobVal * 256);
  //analogWrite(dac5, knobVal * 16);      
  //analogWrite(dac6, knobVal * 16);  
}

// =================================
// Button functions
// =================================

boolean withinBox(int x, int y, int bx, int by, int bw, int bh) {
  if (x >= bx && x <= (bx+bw)) {  // test if X is greater than the box origin x, and less than bx+width
    if (y >= by && y <= (by+bh)) {
      return 1;
    }
  }
  return 0;
}
