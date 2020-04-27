#
# embedXcode
# ----------------------------------
# Embedded Computing on Xcode
#
# Copyright © Rei Vilo, 2010-2020
# All rights reserved
# https://embedXcode.weebly.com
#
# Last update: 19 Dec 2017 release 8.4.2


# Libraries
# ----------------------------------
# Declare standard IDE and user's libraries used
# Short-listing libraries speeds-up building
# Typing = 0 takes none (default for application and user)
# Otherwise, leaving empty considers all (default for local)

# List standard IDE libraries here
# !!! Help: http://bit.ly/2hYLBDC
# default = 0 = none
#
# TEENSY : common = [A] ADC AccelStepper 
#   Adafruit-GFX-Library Adafruit_CC3000 Adafruit_NeoPixel 
#   Adafruit_RA8875 Adafruit_STMPE610 Adafruit_VS1053 
#   Adafruit_nRF8001 AltSoftSerial Artnet Audio [B] Bounce 
#   Bounce2 [C] CapacitiveSensor CryptoAccel [D] DS1307RTC 
#   DmxSimple DogLcd [E] EEPROM EasyTransfer 
#   EasyTransferI2C Encoder Entropy Ethernet [F] FastCRC 
#   FastLED FlexCAN FlexCAN_T4 FlexiTimer2 FreqCount 
#   FreqMeasure FreqMeasureMulti FrequencyTimer2 [G] GSL1680 
#   [I] ILI9341_t3 ILI9488_t3 IRremote [K] Keypad 
#   [L] LedControl LedDisplay LiquidCrystal 
#   LiquidCrystalFast LowPower [M] MFRC522 Metro MsTimer2 
#   [N] NXPMotionSense [O] OSC OctoWS2811 OneWire 
#   [P] PS2Keyboard PWMServo Ping PulsePosition [Q] QuadEncoder 
#   [R] RadioHead ResponsiveAnalogRead [S] SD SPI SPIFlash 
#   ST7565 ST7735_t3 SdFat SerialFlash Servo ShiftPWM 
#   Snooze SoftPWM SoftwareSerial [T] TFT_ILI9163C Talkie 
#   TeensyThreads Time TimeAlarms TimerOne TimerThree 
#   TinyGPS Tlc5940 TouchScreen [U] USBHost_t36 UTFT 
#   [V] VirtualWire [W] WS2812Serial Wire [X] XBee 
#   XPT2046_Touchscreen [a] arduino_midi_library [i] i2c_t3 
#   [k] ks0108 [o] openGLCD [s] ssd1351 [x] x10 
#
APP_LIBS_LIST = Wire SPI USBHost_t36 Encoder Bounce2 EEPROM

# List user's libraries here
# !!! Help: http://bit.ly/2AEscDx
# default = 0 = none
#
# USER = [A] Adafruit-GFX-Library Adafruit_RA8875 [G] GSL1680 
#   [M] MIDIRouter_Lib [S] SdFat [a] arduino_midi_library 
#
USER_LIBS_LIST = SdFat GSL1680 Adafruit-GFX-Library Adafruit_RA8875 arduino_midi_library

# List local libraries here
# !!! Help: http://bit.ly/2juEP91
# default = empty = all
#
LOCAL_LIBS_LIST = MIDIRouter_Library

# List core, application and user's libraries to be excluded
# !!! Help: http://bit.ly/2AijUOI
# For example, WiFi may crash on Arduino 1.0.2, Esplora on Arduino 1.0.3, Firmata on Teensy 3.0,
#   OneWire on MPIDE 0023, HardwareSerial may conflict with MarlinSerial
#
#EXCLUDE_LIBS = Firmata WiFi Esplora OneWire Robot_Control Robot_Control/utility Robot_Motor


# Warning options
# ----------------------------------
# Contrary to errors, warnings don't stop compilation but they point at possible cause of errors.
# !!! Help: http://bit.ly/38hi0NC
# For example, unused variables with unused-variable.
#
# If 0, no warnings
# If empty, all warnings, same as WARNING_OPTIONS = all
# WARNING_OPTIONS = all no-missing-braces no-conversion-null no-pointer-arith
# WARNING_OPTIONS = unused-variable unused-function unused-label unused-value no-conversion-null no-pointer-arith
#
WARNING_OPTIONS = 0


# Compiler options
# ----------------------------------
#
# If 0 or empty, no additional options
# !!! Help: http://bit.ly/2Af7rxq
#
COMPILER_OPTIONS =


# Applications path folder
# ----------------------------------
# Path to the folder where the IDEs are located
# !!! Help: http://bit.ly/2BkdoWV
# By default, = /Applications
#
#APPLICATIONS_PATH = /Applications


# Serial console for Fast target
# ----------------------------------
# For Fast target, open serial console, false or true
# !!! Help: http://bit.ly/2AfyA37
#
NO_SERIAL_CONSOLE = false


# Teensy USB options (default)
# ----------------------------------
# !!! Help: http://bit.ly/2BmH31o
# TEENSY_USB = (USB_SERIAL) USB_HID USB_SERIAL_HID USB_DISK USB_DISK_SDFLASH USB_MIDI
#   USB_RAWHID USB_FLIGHTSIM
# TEENSY_LAYOUT = (LAYOUT_US_ENGLISH) LAYOUT_CANADIAN_FRENCH LAYOUT_CANADIAN_MULTILINGUAL
#   LAYOUT_DANISH LAYOUT_FINNISH LAYOUT_FRENCH LAYOUT_FRENCH_BELGIAN LAYOUT_FRENCH_SWISS
#   LAYOUT_GERMAN LAYOUT_GERMAN_MAC
#   LAYOUT_GERMAN_SWISS LAYOUT_ICELANDIC LAYOUT_IRISH LAYOUT_ITALIAN LAYOUT_NORWEGIAN
#   LAYOUT_PORTUGUESE LAYOUT_PORTUGUESE_BRAZILIAN LAYOUT_SPANISH LAYOUT_SPANISH_LATIN_AMERICA
#   LAYOUT_SWEDISH LAYOUT_TURKISH LAYOUT_UNITED_KINGDOM LAYOUT_US_INTERNATIONAL
#
#TEENSY_USB = USB_SERIAL
#TEENSY_LAYOUT = LAYOUT_US_ENGLISH
TEENSY_USB = USB_MIDI16_SERIAL


# Teensy CPU options (default)
# ----------------------------------
# !!! Help: http://bit.ly/2Bj3c0G
# TEENSY_F_CPU = (96000000) 72000000 48000000 24000000 for Teensy 3.1
# TEENSY_F_CPU = (96000000) 48000000 24000000 for Teensy 3.0
# TEENSY_F_CPU = (48000000) 24000000 for Teensy LC
# TEENSY_F_CPU = (16000000) 8000000 4000000 for Teensy 2.0
# TEENSY_OPTIMISATION = (-Os)
#
#TEENSY_F_CPU = 48000000
#TEENSY_OPTIMISATION = -Os
TEENSY_F_CPU = 180000000
TEENSY_OPTIMISATION = -Os


# Parameters
# ----------------------------------
# Xcode takes BOARD_TAG and BOARD_PORT from the .xcconfig file
# For other IDEs than Xcode,

# BOARD_TAG is required
# If not defined, error BOARD_TAG not defined
#
#BOARD_TAG  = uno

# BOARD_PORT is optional
# !!! Help: http://bit.ly/2yUvq0h
# If not defined, BOARD_PORT = /dev/tty.usb* (default)
#
#BOARD_PORT = /dev/tty.usbmodem*
BOARD_PORT = /dev/cu.usbmodem*

# Xcode takes SKETCHBOOK_DIR from preferences.txt
# If not defined, each IDE has its own SKETCHBOOK_DIR (default)
# To share the same SKETCHBOOK_DIR along all IDEs, define
#
SKETCHBOOK_DIR = ~/Documents/Arduino

# SERIAL_BAUDRATE for the serial console, 9600 by default
# !!! Help: http://bit.ly/2AUEcgp
# Uncomment and specify another speed
#
SERIAL_BAUDRATE = 115200

# OPTIMISATION for the optimisation and debugger levels
# !!! Help: http://bit.ly/2kheSLz
# Uncomment and specify another options
#
#OPTIMISATION    = -Os -g3


# Miscellaneous
# ----------------------------------
# DO NOT EDIT THIS FILE BELOW THIS LINE.
# ----------------------------------
#
EMBEDXCODE_EDITION = embedXcode+ Teensy

# Manage path with space in the name
CURRENT_DIR_SPACE := $(shell pwd)
CURRENT_DIR       := $(shell echo '$(CURRENT_DIR_SPACE)' | sed 's/ /\\\ /g')

# C-compliant project name and extension
PROJECT_NAME_AS_IDENTIFIER = MIDIRouter
SKETCH_EXTENSION   = ino

# Include only current variant of selected platform, false or true
SELECTED_RESOURCES = false

# For distribution, keep main cpp, false or true
KEEP_MAIN_CPP = false

# For building, show details, false or true
HIDE_NUMBER = false
HIDE_COMMAND = false

# For building, keep main and tasks unchanged, false or true
KEEP_MAIN = false
KEEP_TASKS = false

# Local makefiles
#EMBEDXCODE_TOOLS   = $(CURRENT_DIR)/Tools

# Remote makefiles
EMBEDXCODE_TOOLS   = $(HOME)/Library/embedXcode/Tools

MAKEFILE_PATH      = $(EMBEDXCODE_TOOLS)/Makefiles
UTILITIES_PATH     = $(EMBEDXCODE_TOOLS)/Utilities

# Local builds
#BUILDS_PATH_SPACE := $(CURRENT_DIR_SPACE)/Builds
#BUILDS_PATH       := $(CURRENT_DIR)/Builds

# Remote builds
BUILDS_PATH       := $(BUILT_PRODUCTS_DIR)
BUILDS_PATH_SPACE := $(BUILDS_PATH)

include $(MAKEFILE_PATH)/Step1.mk

