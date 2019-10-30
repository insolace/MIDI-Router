
void setup() {
  Serial.begin(115200);
  //  delay(1000);
  if (!SD.begin())
  {
    Serial.println("SD initialization failed!");
    return;
  }

  Serial.println("SD initialization done.");


  // knob
  pinMode(EncSwitch, INPUT_PULLUP);
  encPush.attach(EncSwitch);
  encPush.interval(100);

  // ============================================================
  // EEPROM 
  // ============================================================
  loadEEPROM(); 
  Serial.print("dacNeg[0] = "); Serial.print(dacNeg[0]); Serial.print(" dacPos[0] = "); Serial.println(dacPos[0]); 
  
  // ============================================================
  // DAC 16bit SPI setup
  // ============================================================
  
  // set the CS as an output:
  pinMode (CS, OUTPUT);
  digitalWrite(CS, LOW);

  // initialise SPI:
  SPI2.begin();
  SPI2.setBitOrder(MSBFIRST);
  
  //enable internal ref
  digitalWriteFast(CS, LOW);
  int c1 = B00000001;
  int c2 = B00010000;
  int c3 = B00000000;
  SPI2.transfer(c1);
  SPI2.transfer(c2);
  SPI2.transfer(c3);
  digitalWriteFast(CS, HIGH);

  setDAC(6, dacNeg[0]);

  // ============================================================
  // DAC 12bit internal setup
  // ============================================================
  
  pinMode(dac5, OUTPUT); 
  pinMode(dac6, OUTPUT);   
  analogWriteResolution(12);

  // ============================================================
  // D 1-6 setup
  // ============================================================
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(dig4, OUTPUT);
  pinMode(dig5, OUTPUT);
  pinMode(dig6, OUTPUT);
  pinMode(dig1, OUTPUT);
  //pinMode(adc1, INPUT);
  //pinMode(adc2, INPUT);
  
  
  // ============================================================
  // MIDI setup
  // ============================================================

  MIDI1.begin(MIDI_CHANNEL_OMNI);
  MIDI2.begin(MIDI_CHANNEL_OMNI);
  MIDI3.begin(MIDI_CHANNEL_OMNI);
  MIDI4.begin(MIDI_CHANNEL_OMNI);
  MIDI5.begin(MIDI_CHANNEL_OMNI);
  MIDI6.begin(MIDI_CHANNEL_OMNI);

  MIDI1.turnThruOff();
  MIDI2.turnThruOff();
  MIDI3.turnThruOff();
  MIDI4.turnThruOff();
  MIDI5.turnThruOff();
  MIDI6.turnThruOff();

  // USB Devices hosted by TEENSY
  // Wait 1.5 seconds before turning on TEENSY USB Host.  If connected USB devices
  // use too much power, Teensy at least completes USB enumeration, which
  // makes isolating the power issue easier.
  delay(1500);
  Serial.println("Interface_16x16 Example");
  delay(10);
  myusb.begin();

// ============================================================
// Touchscreen setup
// ============================================================

  // Setup Touch
  TS.begin(WAKE, INTRPT);                 // Startup sequence CONTROLER part
  pinMode(led, OUTPUT);

  Serial.println("RA8875 start");
  while (!Serial && (millis() <= 1000));

  /* Initialise the display using 'RA8875_480x272' or 'RA8875_800x480' */
  while (!tft.begin(RA8875_800x480))  {
    Serial.println("RA8875 Not Found!");
    delay(100);
  }
  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);

  // Rotation
  tft.setRotation(curRot);
  //tft.setRotationA(curRot);
    
  // With hardware accelleration this is instant
  tft.fillScreen(RA8875_BLACK);
  tft.graphicsMode();
  bmpDraw("WELCOME.BMP", 0, 0);
  delay(1000);
/*
  
 
  // Play with PWM
  for (uint8_t i = 255; i != 0; i -= 5 ) {
    tft.PWM1out(i);
    delay(10);
  }
  for (uint8_t i = 0; i != 255; i += 5 )  {
    tft.PWM1out(i);
    delay(10);
  }
  tft.PWM1out(255);

  //set rotate
  curRot = 2;
  tft.setRotation(curRot);

  delay(200);
  tft.fillScreen(RA8875_YELLOW);
  delay(500);
  tft.fillScreen(RA8875_GREEN);
  delay(500);
  tft.fillScreen(RA8875_CYAN);
  delay(500);
  tft.fillScreen(RA8875_MAGENTA);
  delay(500);
  tft.fillScreen(RA8875_BLACK);
*/
  tft.touchEnable(false);

  randomSeed(analogRead(0));
  tft.textMode();
  tft.textEnlarge(2);

  // end touchscreen setup

  // setup homescreen

  drawBox();
  drawBGs();
  drawRows();
  drawColumns();
  drawRouting();
 

}
