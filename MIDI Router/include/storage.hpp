#ifndef STORAGE_H
#define STORAGE_H

#include <EEPROM.h>
#include "main.hpp"

extern char syIdHex[20]; ///< Column 1, SysEx ID header in HEX
extern char mfg[80]; ///< Column 2, manufacturer name (text/ASCII)

#ifdef SDCARD_SUPPORT

#include <SD.h>
#include <SPI.h>


/// CSV_DELIM CSV file field delimiter
#define CSV_DELIM ','



extern File SysCsvFile;

void initSD();

/// Read text from CSV
/// @param SysCsvFile File
/// @param str Pointer to store/return text
/// @param size Size
/// @param delim Character separator (",")
/// @return int value
int csvReadText(File* SysCsvFile, char* str, size_t size, char delim);

/// Read 32 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadInt32(File* SysCsvFile, int32_t* num, char delim);

/// Read 16 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadInt16(File* SysCsvFile, int16_t* num, char delim);

/// Read uint 32 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim);

/// Read uint 16 bit value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim);

/// Read double value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadDouble(File* SysCsvFile, double* num, char delim);

/// Read floating point value from CSV
/// @param SysCsvFile File
/// @param num Pointer to store data
/// @param delim Character separator (",")
/// @return int value
int csvReadFloat(File* SysCsvFile, float* num, char delim);

/// Close CSV File
void csvClose();





/// matchSysExID
/// @param b1 Byte 1
/// @param b2 Byte 2
/// @param b3 Byte 3
void matchSysExID(int16_t b1, int16_t b2, int16_t b3);
/// Debugging - print the matched SysEx ID to the Serial port
void printMatch();

#endif // #ifdef SDCARD_SUPPORT

// EEPROM
void saveEEPROM(); ///< save to eeprom
void loadEEPROM(); ///< load from eeprom

#endif/* STORAGE_H */
