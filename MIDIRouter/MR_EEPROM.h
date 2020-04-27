/*

MIDI Router
Created by Eric Bateman (eric at timeline85 dot com)
http://www.midirouter.com
 
MR_EEPROM.h - MIDI Router EEPROM functions
Source code written by Eric Bateman with contributions from Kurt Arnlund
Copyright © 2020 Eric Bateman and Kurt Arnlund. All rights reserved.

License:GNU General Public License v3.0
 
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
