#ifndef LCDBUTTONS_H
#define LCDBUTTONS_H

#include "dtd.h"

// Low-level button press
int readLCDbuttons() {  // button pressed? return its value
// following literal depends upon ADC clock divider.
int keyValue, counter=3000; // gives a rough "one thousand and one" delay...
uint8_t button = btnNONE;

    // my buttons when read return these values: 0, 132, 306, 478, 720
    // the arithmetic halfway points are:          66  219  392  599  750
    
  keyValue = analogRead(btnADC); // read value from sensor

  if (keyValue < 1000) {

    if (keyValue < 65)button = btnRIGHT;
    else if (keyValue < 220)button = btnUP;
    else if (keyValue < 390)button = btnDOWN;
    else if (keyValue < 600)button = btnLEFT;
    else if (keyValue < 750)button = btnSELECT;
    else return btnNONE;

    while (analogRead(btnADC) < 1000) {
      if(!--counter)
        return button;    // timed out, do buttonpress and return for more. (allows autorepeat)
        }  // wait here for release, unless we timeout
  }
  return button;
}

#endif