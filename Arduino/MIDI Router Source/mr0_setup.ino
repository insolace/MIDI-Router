
void setup() {
  Serial.begin(115200);
  //  delay(1000);

  // knob
  pinMode(EncSwitch, INPUT_PULLUP);
  debouncer.attach(EncSwitch);
  debouncer.interval(100);
  
  
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
  tft.fillScreen(RA8875_WHITE);
  tft.graphicsMode();

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

