#include "storage.hpp"

#include "main.hpp"
#include "cvDAC.hpp"
#include "midi.hpp"
#include "touchscreen.hpp"

// EEPROM
int eeprom_addr_offset = 0; ///< Create address offset so Array2 is located after dacOffset in EEPROM

// Variables for storing and reading SysEx/CSV from SD card
char syIdHex[20]; ///< Column 1, SysEx ID header in HEX
char mfg[80]; ///< Column 2, manufacturer name (text/ASCII)
int16_t idLen; ///< Column 3, the length of the SysEx ID in bytes
int16_t idB1; ///< Column 4, ID byte 1
int16_t idB2; ///< Column 5, ID byte 2
int16_t idB3; ///< Column 6, ID byte 3


#ifdef SDCARD_SUPPORT

#include <SD.h>
#include <SPI.h>

// set up variables using the SD utility library functions:
Sd2Card card;
SdVolume volume;
File SysCsvFile;

const int chipSelect = BUILTIN_SDCARD;


void initSD()
{


    Serial.print("Initializing SD card...");

    // we'll use the initialization code from the utility libraries
    // since we're just testing if the card is working!
    if (!card.init(SPI_HALF_SPEED, chipSelect)) {
        Serial.println("initialization failed. Things to check:");
        Serial.println("* is a card inserted?");
        Serial.println("* is your wiring correct?");
        Serial.println("* did you change the chipSelect pin to match your shield or module?");
        return;
    } else {
        Serial.println("Wiring is correct and a card is present.");
    }

    // print the type of card
    Serial.print("\nCard type: ");
    switch(card.type()) {
        case SD_CARD_TYPE_SD1:
        Serial.println("SD1");
        break;
        case SD_CARD_TYPE_SD2:
        Serial.println("SD2");
        break;
        case SD_CARD_TYPE_SDHC:
        Serial.println("SDHC");
        break;
        default:
        Serial.println("Unknown");
    }

    if (!SD.begin(chipSelect)) {
        Serial.println("initialization failed!");
        return;
    }

    Serial.println("initialization done.");

    if (!volume.init(card)) {
        Serial.println("Could not find FAT16/FAT32 partition.\nMake sure you've formatted the card");
        return;
    }

    // Open the file.
    SysCsvFile = SD.open("sysexids/sysexids.csv", FILE_WRITE);
    if (!SysCsvFile)
    {
        Serial.println("open failed");
        return;
    }

}




// ================================================================================
// SysEx ID lookup
// ================================================================================

void matchSysExID(int16_t b1, int16_t b2, int16_t b3)
{
    SysCsvFile.seek(156); // skip first 3 lines of Sysex Table

    while (SysCsvFile.available())
    {

        if (csvReadText(&SysCsvFile, syIdHex, sizeof(syIdHex), CSV_DELIM) != CSV_DELIM
                || csvReadText(&SysCsvFile, mfg, sizeof(mfg), CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idLen, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB1, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB2, CSV_DELIM) != CSV_DELIM
                || csvReadInt16(&SysCsvFile, &idB3, CSV_DELIM) != CSV_DELIM)
        {
            Serial.println("read error");
            Serial.print("Pos: "); Serial.println(SysCsvFile.position());
            int ch;
            int nr = 0;
            // print part of file after error.
            while ((ch = SysCsvFile.read()) > 0 && nr++ < 100)
            {
                Serial.write(ch);
                Serial.print("("); Serial.print(nr); Serial.print(")");
            }
            break;
        }
        if ((b1 == 0 && idB1 == 0 && idB2 == b2 && idB3 == b3)
                || ((b1 != 0) && (b1 == idB1)))
        {
            printMatch();
            break;
        }

    }
}

void printMatch()
{
    Serial.print("MATCH: ");
    Serial.print(syIdHex);
    Serial.print(CSV_DELIM);
    Serial.print(mfg);
    Serial.print(CSV_DELIM);
    Serial.print(idLen);
    Serial.print(CSV_DELIM);
    Serial.print(idB1);
    Serial.print(CSV_DELIM);
    Serial.print(idB2);
    Serial.print(CSV_DELIM);
    Serial.println(idB3);
}

// ================================================================================
// CSV
// ================================================================================


int csvReadText(File* SysCsvFile, char* str, size_t size, char delim)
{
    char ch;
    int rtn;
    size_t n = 0;
    while (true)
    {
        // check for EOF
        if (!SysCsvFile->available())
        {
            rtn = 0;
            break;
        }
        if (SysCsvFile->read(&ch, 1) != 1)
        {
            // read error
            rtn = -1;
            break;
        }
        // Delete CR.
        if (ch == '\r')
        {
            continue;
        }
        if (ch == delim || ch == '\n')
        {
            rtn = ch;
            break;
        }
        if ((n + 1) >= size)
        {
            // string too long
            rtn = -2;
            n--;
            break;
        }
        str[n++] = ch;
    }
    str[n] = '\0';
    return rtn;
}

// MARK: - CSV Reading

int csvReadInt32(File* SysCsvFile, int32_t* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtol(buf, &ptr, 10);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadInt16(File* SysCsvFile, int16_t* num, char delim)
{
    int32_t tmp;
    int rtn = csvReadInt32(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    if (tmp < INT_MIN || tmp > INT_MAX)
    {
        return -5;
    }
    *num = tmp;
    if (rtn == 10)
    {
        return 44;    // convert NL to comma
    }
    return rtn;
}

int csvReadUint32(File* SysCsvFile, uint32_t* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtoul(buf, &ptr, 10);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadUint16(File* SysCsvFile, uint16_t* num, char delim)
{
    uint32_t tmp;
    int rtn = csvReadUint32(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    if (tmp > UINT_MAX)
    {
        return -5;
    }
    *num = tmp;
    return rtn;
}

int csvReadDouble(File* SysCsvFile, double* num, char delim)
{
    char buf[20];
    char* ptr;
    int rtn = csvReadText(SysCsvFile, buf, sizeof(buf), delim);
    if (rtn < 0)
    {
        return rtn;
    }
    *num = strtod(buf, &ptr);
    if (buf == ptr)
    {
        return delim;
    }
    while (isspace(*ptr))
    {
        ptr++;
    }
    return *ptr == 0 ? rtn : -4;
}

int csvReadFloat(File* SysCsvFile, float* num, char delim)
{
    double tmp;
    int rtn = csvReadDouble(SysCsvFile, &tmp, delim);
    if (rtn < 0)
    {
        return rtn;
    }
    // could test for too large.
    *num = tmp;
    return rtn;
}

void csvClose()
{
    SysCsvFile.close();
}

#endif //#ifdef SDCARD_SUPPORT

// ================================================================================
// EEPROM
// ================================================================================



void saveEEPROM()
{
    eeprom_addr_offset = 0;
    //Write dacNeg
    EEPROM.put(eeprom_addr_offset, dacNeg);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacNeg);

    //Write dacPos
    EEPROM.put(eeprom_addr_offset, dacPos);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);

    //Write dacOffset
    EEPROM.put(eeprom_addr_offset, dacOffset);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);

    //Write clearRouting
    EEPROM.put(eeprom_addr_offset, clearRouting);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(clearRouting);

    //Write routing
    EEPROM.put(eeprom_addr_offset, routing);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);

}

void loadEEPROM()
{
    eeprom_addr_offset = 0;
    //Read dacNeg
    EEPROM.get(eeprom_addr_offset, dacNeg);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacNeg);

    //Read dacPos
    EEPROM.get(eeprom_addr_offset, dacPos);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);

    //Read dacOffset
    EEPROM.get(eeprom_addr_offset, dacOffset);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);

    //Read clearRouting;
    EEPROM.get(eeprom_addr_offset, clearRouting);
    eeprom_addr_offset = eeprom_addr_offset + sizeof(clearRouting);

    //Read routing, maybe
    if (clearRouting == pi)    // 1
    {
        Serial.println("pi");
        EEPROM.get(eeprom_addr_offset, routing);
        eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);
    }
    else
    {
        clearRouting = pi;
        saveEEPROM();
    }
}

