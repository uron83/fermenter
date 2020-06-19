/*
  Controls.h - Library for controlling 5 analog input button.
*/
#ifndef LcdKeypad_h
#define LcdKeypad_h

#include "Arduino.h"
#include <LiquidCrystal.h>

class LcdKeypad{
  public:
    LcdKeypad(int dummy);
    void setup();
    void setCursor(int x, int y);
    void print(char* value);
    void print(int value);
    void print(float value);
    bool isRightPressed();
    bool isUpPressed();
    bool isDownPressed();
    bool isLeftPressed();
    bool isSelectPressed();
    bool isAnyPressed();
    int readValue();
    void backlightOn();
    void backlightOff();
    void autoToggleBacklight();
    void printArrowUp(int x, int y);
    void printArrowDown(int x, int y);
    void printEmpty(int x, int y);
  private:
    //LCD pin to Arduino
    const int pin_a0 = 0;
    const int pin_RS = 8;
    const int pin_EN = 9;
    const int pin_d4 = 4;
    const int pin_d5 = 5;
    const int pin_d6 = 6;
    const int pin_d7 = 7;
    const int pin_BL = 10;
    int value;
    const unsigned int backlightTimeout = 60000;
    unsigned long blTrunedOn;
    LiquidCrystal *lcd;
};

extern LcdKeypad lcdKeypad;

#endif
