void loop() {


  readKnob();  // check knob for turn/push
  touchIO();   // process touchscreen input
  routeMidi(); // check incoming MIDi and route it
  
//**************************

// test for active input flashes, turn off after interval
/*  for (int i = 0; i < 6; i++){
    if (inFlag[i] = 1) {
      if ((elapseIn) > flashTime) {
        inFlag[i] = 0;
        flashIn(i, 0);
      }      
    }
  }
*/
  
} // end main loop

