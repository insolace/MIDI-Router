void setup() {
  Serial.begin(115200);
  //  delay(1000);
  if (!SD.begin())
  {
    Serial.println("SD initialization failed!");
    //return;
  } else {
    Serial.println("SD initialization done.");
  }

  // knob
  pinMode(EncSwitch, INPUT_PULLUP);
  encPush.attach(EncSwitch);
  encPush.interval(100);

  // ============================================================
  // EEPROM 
  // ============================================================
  loadEEPROM(); 
  
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
  //pinMode(adc1, INPUT);  // hardware is there but not implemented yet in software
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
  delay(150);
  myusb.begin();

// ============================================================
// Touchscreen setup
// ============================================================

  // Setup Touch
  TS.begin(WAKE, INTRPT);                 // Startup sequence CONTROLER part
  pinMode(led, OUTPUT);

  //Serial.println("RA8875 start");
  while (!Serial && (millis() <= 1000));

  // Initialise the display 
  while (!tft.begin(RA8875_800x480))  {
    Serial.println("RA8875 Not Found!");
    delay(100);
  }
  Serial.println("Found RA8875");

  tft.displayOn(true);
  tft.GPIOX(true);                              // Enable TFT - display enable tied to GPIOX
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
  tft.PWM1out(255);                             // Set backlight brightness

  // Rotation
  tft.setRotation(curRot);
  //tft.setRotationA(curRot);
    
  // With hardware accelleration this is instant
  tft.fillScreen(RA8875_BLACK);
  tft.graphicsMode();
  bmpDraw("WELCOME.BMP", 0, 0);
  delay(1000);
  tft.fillScreen(RA8875_BLACK);
  tft.touchEnable(false);

  randomSeed(analogRead(0));
  tft.textMode();
  tft.textEnlarge(2);

  // ============================================================
  // MIDI interrupt timer
  // ============================================================
  callMIDI.begin(routeMidi, INTERVALMIDI);

  // ============================================================
  // Sysex CSV stuff
  // ============================================================
  
  if (!SD.chdir("sysexids")) {
    Serial.println("chdir failed for Folder1.\n");
  }
  
  // Open the file.
  SysCsvFile = SD.open("sysexids.csv", FILE_WRITE);
  if (!SysCsvFile) {
    Serial.println("open failed");
    return;
  }

  // sysex id req
  delay(250);  // allow send/receive buffers to settle, some MIDI devices are chatty when powered on 
  profileInstruments();
  delay(250);  // wait for responses

  
  // draw homescreen
  drawHomeScreen();
  rdFlag = 0;

}
  
