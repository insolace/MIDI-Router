// =============================
// draw globals box
// =============================
void drawBox() {

  // Settings/Home
  tft.fillRect(hbOX, hbOY, hbWidth, hbHeight, newColor(hbColor));
  
  // inputs page select Background
  tft.fillRect(tbOX, 0, tbWidth, hbHeight, newColor(ibColor)); 
  curX = tbOX + 1;
  curY = 5;
  dPrint("Inputs");
    
  // outputs page select Background
  tft.fillRect(0, hbHeight, hbWidth, tbHeight, newColor(obColor)); 
  curX = 0;
  curY = hbHeight * 1.6;
  dPrint("Outputs");
  
  // Clock/Tempo Box
  tft.fillRect(tbOX, tbOY, tbWidth, tbHeight, newColor(tbColor));

}


// =============================
// draw rows and columns
// =============================


void drawColumns() {
  tft.fillRect(cOffset, 0, WIDE - cOffset, rOffset, newColor(insColor));
  tft.setTextColor(newColor(txColor), newColor(insColor));

  for (int i = 0; i < columns; i++) {
    tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), TALL, newColor(linClr));

    curY = (i*cWidth) + tCOffset;
    curX = tBord;
    tft.setRotation(3);
    dPrint(inputNames[(5-i)+(pgIn *6)]);
    tft.setRotation(curRot);
  }
  
}

void drawRows() {

  tft.fillRect(0, rOffset, cOffset, TALL - rOffset, newColor(outsColor));
  
  for (int i = 0; i < rows; i++) {
    tft.textColor(newColor(txColor), newColor(outsColor));
    //tft.textSetCursor(0, rOffset + (i*rHeight));
    curX = tBord;
    curY = rOffset + (i*rHeight) + tROffset;
    dPrint(outputNames[i+(pgOut *6)]); 
    tft.drawLine(0, rOffset + ((i)*rHeight), WIDE, rOffset + ((i)*rHeight), newColor(linClr));
  }
}

void drawRouting() {
  for (int i = 0; i < rows; i++) {
    for (int c = 0; c < columns; c++) {
      if (routing[c + (pgIn * 6)][i + (pgOut * 6)] == 1) {
        tft.fillRect(cOffset + (cWidth * c), rOffset + (rHeight * i), cWidth, rHeight, newColor(routColor)); 
      } else {
        tft.fillRect(cOffset + (cWidth * c), rOffset + (rHeight * i), cWidth, rHeight, newColor(gridColor)); 
      }          
    }  
  }
  drawGLines();
}

// Draw grid lines
void drawGLines() {
  for (int i = 0; i < rows; i++) {
    tft.drawLine(0, rOffset + ((i)*rHeight), WIDE, rOffset + ((i)*rHeight), newColor(linClr));
  }
  for (int i = 0; i < columns; i++) {
    tft.drawLine(cOffset + ((i)*cWidth), 0, cOffset + ((i)*cWidth), TALL, newColor(linClr));
  }
}

// =============================
// draw backgrounds; grid, inputs and outputs
// =============================

void drawBGs() {
    //tft.fillRect(cOffset, rOffset, WIDE,TALL, newColor(gridColor));  // grid bg    
    tft.fillRect(cOffset, 0, WIDE-cOffset, rOffset, newColor(insColor));  // inputs bg  
    tft.fillRect(0, rOffset, cOffset, TALL-rOffset, newColor(outsColor));  // outputs bg
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
    tft.fillRect( cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, newColor(insColFlash)); 
    //(*elapsedIn+inp) = 0;
    inFlag[inp] = 1;
  } else {
    // draw input background
    tft.fillRect( cOffset + (cWidth * 6) + 1, 0, cWidth - 1, rOffset - 1, newColor(insColor)); 
  }

  // redraw input name
  tft.textColor(newColor(txColor), newColor(insColor));
  tft.textSetCursor(cOffset + (inp*cWidth) + tCOffset , rOffset - tROffset);

}

