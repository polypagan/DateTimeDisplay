#include <Arduino.h>
/*
 * DateTimeDisplay sketch
 * 
 * Inspired by http://educ8s.tv/arduino-project-real-time-clock-with-ds3231-module/
 * (and I haven't done much with Mega.)
 * 
 * DdelV 20210322
 * 
 * Hardware:
 *  Arduino Mega2560 (clone)
 *  Display shield (16x2, with buttons)
 *  RTC chip (Found DS3132 in inventory)
 * 
 * Software
 *  Shows live time/date
 *  Time/date set mode, features
 *  Other RTC features? Temperature
 */

#include <Wire.h>
#include <ds3231.h>   // this library is archived. Might be well to find another (Adafruit?)
                      // needed: init, get, set. additionally, struct ts may need to be replaced.
#include <LiquidCrystal.h>

LiquidCrystal lcd(8,9,4,5,6,7); 

#define DEBUG false

#include "dtd.h"
#include "LCDbuttons.h"

// display update interval
const unsigned long INTERVAL = 1000;

// converts index to cursor position
int ciLookup(int i) {     // could add checking, but no real harm
const uint8_t ilu[] = { 3, 6, 9, 12, 15 };

  return ilu[i];
}

// display timedate record with cursor addressing value to change
void showCursor(int i, struct ts t) {
  const int BUFMAX = 18;
  char buff[BUFMAX];

// show datetime in new orientation:
  snprintf(buff, BUFMAX, "%4d/%02d/%02d %02d:%02d",
                          t.year, t.mon, t.mday, t.hour, t.min);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(buff);
  lcd.setCursor(ciLookup(i),1);
  lcd.print("^");
}

// increment value at cursor, with care for overflow possibilities
void incVal(int i, struct ts *t){
#if DEBUG
const int BUFMAX = 18;
char buff[BUFMAX];
#endif

// table of days in month
const uint8_t mmax[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#if DEBUG
    Serial.print("incVal(");
    Serial.print(i);
    Serial.print(", ");
    snprintf(buff, BUFMAX, "%4d/%02d/%02d %02d:%02d", t->year, t->mon, t->mday, t->hour, t->min);
    Serial.print(buff);
    Serial.println(")");
#endif

  switch (i) {

    case 0 :  // year
      t->year++; // no limit that we know of
      break;

    case 1 :  // month
      if (++t->mon > 12) t->mon = 1;
      break;

    case 2 :  // day of month
      if (++t->mday > mmax[t->mon]) t->mday = 1;
      break;

    case 3 : // hour
      if (++t->hour > 23) t->hour = 0;
      break;

    case 4 :  // minute
      if (++t->min > 59) t->min = 0;
      break;

    default :
      Serial.println("ERROR: incVal(), parameter out of bounds!");
      while (1) { delay(1); }
  }
}

// decrement value at cursor, with care for underflows (watch out for unsigned values)
void decVal(int i, struct ts *t) {

// table of days in each month
const uint8_t mmax[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  switch (i) {

    case 0 :  // year
      --t->year; // no limit that we know of
      break;

    case 1 :  // month
      if (--t->mon < 1) t->mon = 12;
      break;

    case 2 :  // day of month
      if (--t->mday < 1) t->mday = mmax[t->mon];
      break;

    case 3 : // hour
      if (t->hour == 1) t->hour = 23;  // unsigned value
      else --t->hour;
      break;

    case 4 :  // minute
      if (t->min == 1) t->min = 59;    // same potential problem here
      else --t->min;
      break;

    default :
      Serial.println("ERROR: decVal(), parameter out of bounds!");
      while (1) { delay(1); }
  }
}

// Time/Date setting requested. This does the UI and update.
void setTime(struct ts t) {
  int i = mfield;      // come prepared to alter minutes field.

// display Date (in numeric format) and time.
// use LEFT/RIGHT buttons to select digit to change
// and UP/DOWn to increment/decrement (carefully!),
// on SELECT, set new time and return to caller.

/* 
 *          1
 * 1234567890123456
 * YYYY/MM/DD HH:MM
 *          ^       <-- moving cursor
 *    4  7 10 13 16 <-- valid index
 *    ~ 12 31 23 59 <-- max val (but DD needs lookup)
 */
  showCursor(i, t);     // prime pump by showing user default cursor

  while (true) {
    switch (readLCDbuttons()) {
      case btnSELECT :
        t.sec = 30;       // split the difference
        DS3231_set(t);    // store altered struct
        return;
        break;            // unreachable break

      case btnLEFT :
        if (--i < Yfield) i = mfield;     // just wrap
        showCursor(i,t);
        break;

      case btnRIGHT :
        if (++i > mfield) i = Yfield;     // just wrap
        showCursor(i, t);
        break;

      case btnUP :
        incVal(i,&t);                     // pass struct to be modified
        showCursor(i, t);
        break;

      case btnDOWN :
        decVal(i,&t);                     // pass struct to be modified
        showCursor(i, t);
        break;

      default :             // just eat all else (nothing pressed)
        break;

    }
  }
}


// new version (based on void printMonth(int Month)) returns const char* for formatted printing.
const char* Month(int month)
{
  switch(month)
  {
    case 1: return (" January ");break;
    case 2: return (" February ");break;
    case 3: return (" March ");break;
    case 4: return (" April ");break;
    case 5: return (" May ");break;
    case 6: return (" June ");break;
    case 7: return (" July ");break;
    case 8: return (" August ");break;
    case 9: return (" September");break;
    case 10: return (" October ");break;
    case 11: return (" November ");break;
    case 12: return (" December ");break;
    default: return (" Error ");break;
  } 
}

void setup() {
  // init Serial for debug,
  // Init display, clock.
  Serial.begin(115200);
  Serial.println("\n\n\nDateTimeDisplay");

  pinMode(10, OUTPUT);
  analogWrite(10, 32);    // dim the backlight for comfortable viewing...

  Wire.begin();
  DS3231_init(DS3231_CONTROL_INTCN);

  lcd.begin(16, 2);
  lcd.clear();
}

// The running Clock/Calendar/Thermometer Display
void showDisplay(struct ts t, float temperature) {
char line[18];  // one extra
char tempF[6];
int ccnt;

dtostrf(temperature, 5, 1, tempF);

ccnt = snprintf(line, 18, "%2d%s%4d", t.mday, Month(t.mon), t.year);
lcd.clear();
lcd.setCursor((16-ccnt)/2,0); // center the line (month name length varies).
lcd.print(line);
Serial.println(line);

snprintf(line, 18, "%02d:%02d:%02d %s%cC", t.hour, t.min, t.sec, tempF, (char)223);
lcd.setCursor(0,1);
lcd.print(line);
Serial.println(line);

}

void loop() {
static unsigned long prev;
struct ts t;

  // Read time and date,
  // Display same,
  // If mode pressed, enter set mode.
  
  while ((millis() - prev) < INTERVAL) {
    if (readLCDbuttons() == btnSELECT) {
      DS3231_get(&t); //Get time
      while (readLCDbuttons() != btnNONE) {   // wait for release (move to setTime()?)
        delay(1); 
      }
      setTime(t);
    }
  }
  prev = millis();
  DS3231_get(&t); //Get time
  showDisplay(t, DS3231_get_treg());
}