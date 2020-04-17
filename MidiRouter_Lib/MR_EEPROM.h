//
//  MR_EEPROM.h
//  MidiRouter_Lib
//
//  Created by Kurt Arnlund on 4/16/20.
//  Copyright © 2020 Kurt Arnlund. All rights reserved.
//

#ifndef MR_EEPROM_h
#define MR_EEPROM_h

void saveEEPROM() {
    eeprom_addr_offset = 0;
    //Write dacNeg
    EEPROM.put( eeprom_addr_offset, dacNeg );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacNeg);
    
    //Write dacPos
    EEPROM.put( eeprom_addr_offset , dacPos );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);
    
    //Write dacOffset
    EEPROM.put( eeprom_addr_offset , dacOffset );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);
    
    //Write clearRouting
    EEPROM.put(eeprom_addr_offset, clearRouting );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(clearRouting);
    
    //Write routing
    EEPROM.put( eeprom_addr_offset , routing );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);
    
}

void loadEEPROM() {
    eeprom_addr_offset = 0;
    //Read dacNeg
    EEPROM.get( eeprom_addr_offset, dacNeg );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacNeg);
    
    //Read dacPos
    EEPROM.get( eeprom_addr_offset , dacPos );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);
    
    //Read dacOffset
    EEPROM.get( eeprom_addr_offset , dacOffset );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);
    
    //Read clearRouting;
    EEPROM.get( eeprom_addr_offset, clearRouting );
    eeprom_addr_offset = eeprom_addr_offset + sizeof(clearRouting);
    
    //Read routing, maybe
    if (clearRouting == pi) {  // 1
        Serial.println("pi");
        EEPROM.get( eeprom_addr_offset , routing );
        eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);
    } else {
        clearRouting = pi;
        saveEEPROM();
    }
}

#endif /* MR_EEPROM_h */
