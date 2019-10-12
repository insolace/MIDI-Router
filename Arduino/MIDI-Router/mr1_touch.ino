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
          evaltouch();

          //drawBox();
          //drawBGs();
          //drawRows();
          //drawColumns();             
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
  if (touchY <= rOffset && touchX >= cOffset) {
    // input selected
  } else if (touchY >= rOffset && touchX <= cOffset) {
    // output selected
  } else if (touchY >= rOffset && touchX >= cOffset) {
    // routing grid selected
    curGrid = routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)];
    if (curGrid == 0){
      routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)] = 1;
    } else {
      routing[getTouchCol(touchX)-1 + (pgIn * 6)][getTouchRow(touchY)-1 + (pgOut * 6)] = 0;
    }
    drawRouting();
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
    // setup/home box
    drawRows();
  }  
}

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
      //Serial.print("Dif: ");
      //Serial.println(kSpeed);
      if (abs(kSpeed) > knobSpeedup) {   
        newPosition = newPosition + (knobSpeedRate * kSpeed); 
        myEnc.write(newPosition);
      }

      oldPosition = newPosition;
      knobVal = newPosition / 4; // four pulses per encoder click, scale down so 1 knob val = 1 click
      if (oldKnobVal != knobVal) { // is the scaled value new?
        // update knob value and DO SOMETHING
        oldKnobVal = knobVal;
        cveeKnobOffset = (knobVal - 128) * 10;
        Serial.println(cveeKnobOffset);  
        //tft.PWM1out(knobVal);  // dim screen 
        //setDAC(4, knobVal * 256);
        //analogWrite(dac5, knobVal * 16);      
        //analogWrite(dac6, knobVal * 16); 
      }

    }
  }

  // encoder switch
  debouncer.update();
  if ( debouncer.fell() ) {
    Serial.println("push");  // knob pushed, do something

    if (knobVal > (knobMax/2) || knobVal == knobMin) { // change to max value
        //knobVal = knobMax; 
        //oldKnobVal = knobMin;
        myEnc.write(knobMax * 4);
        //oldPosition = knobMin * 4;
    } else if (knobVal < (knobMax/2) || knobVal == knobMax) {  // change to min value
        //knobVal = knobMin; 
        //oldKnobVal = knobMax;
        myEnc.write(knobMin * 4);
        //oldPosition = knobMax * 4;   
    }
    Serial.println(myEnc.read());  // knob pushed, do something
  }
}
