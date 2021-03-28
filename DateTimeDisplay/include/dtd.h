#ifndef DTD_H
#define DTD_H

const uint8_t btnADC = A0;

enum {      // tokens returned by readLCDbuttons()
  btnNONE,
  btnUP,
  btnDOWN,
  btnLEFT,
  btnRIGHT,
  btnSELECT
};

enum {      // values taken by index i to ilu[]
  Yfield = 0,
  Mfield,
  Dfield,
  Hfield,
  mfield
};

#endif