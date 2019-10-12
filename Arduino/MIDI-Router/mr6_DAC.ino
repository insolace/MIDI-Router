void setDAC(int dac, uint32_t data) {
  int c1, c2, c3 = 0;
  switch (dac) {
    case 0:
        c1 = dacA;
        break;
    case 1:
        c1 = dacB;
        break;
    case 2:
        c1 = dacC;
        break;
    case 3:
        c1 = dacD;
        break;
    case 4:
        c1 = dALL;
        break;
  }
    
  digitalWriteFast(CS, LOW);

  c2 = data >> 8;
  c3 = data;

  SPI2.transfer(c1);
  SPI2.transfer(c2);
  SPI2.transfer(c3);

  digitalWriteFast(CS, HIGH);
}
