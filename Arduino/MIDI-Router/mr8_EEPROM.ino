void saveEEPROM() {
  //Write dacNeg
  EEPROM.put( 0, dacNeg );
  eeprom_addr_offset = sizeof(dacNeg);
    
  //Write dacPos
  EEPROM.put( eeprom_addr_offset , dacPos );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);

  //Write dacOffset
  EEPROM.put( eeprom_addr_offset , dacOffset );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);
  
  //Write routing
  EEPROM.put( eeprom_addr_offset , routing );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);
  
}

void loadEEPROM() {
  //Read dacNeg
  EEPROM.get( 0, dacNeg );
  eeprom_addr_offset = sizeof(dacNeg);
    
  //Read dacPos
  EEPROM.get( eeprom_addr_offset , dacPos );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(dacPos);

  //Read dacOffset
  EEPROM.get( eeprom_addr_offset , dacOffset );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(dacOffset);
  
  //Read routing
  EEPROM.get( eeprom_addr_offset , routing );
  eeprom_addr_offset = eeprom_addr_offset + sizeof(routing);
}
