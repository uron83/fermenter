/*
  Controls.cpp
*/

#include "Arduino.h"
#include "LcdKeypad.h"

#define SafeBLon(pin) pinMode(pin, INPUT)
#define SafeBLoff(pin) pinMode(pin, OUTPUT)
#define CharUp byte(0)
#define CharDown byte(1)
#define CharPause byte(2)

byte charUp[8] = {
  B00100,
  B01110,
  B11111,
  B00100,
  B00100,
  B00100,
  B00100,
  B00000,
};

byte charDown[8]
{
  B00000,
  B00100,
  B00100,
  B00100,
  B00100,
  B11111,
  B01110,
  B00100,
};

byte charPause[8]
{
  B00000,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B00000,
};

LcdKeypad::LcdKeypad(int dummy) {

	this->lcd = new LiquidCrystal(pin_RS, pin_EN, pin_d4, pin_d5, pin_d6, pin_d7);

	pinMode(pin_a0, INPUT);

}

bool LcdKeypad::isRightPressed() {
	return this->value < 60;
}

bool LcdKeypad::isUpPressed() {
	return this->value >= 60 && this->value < 200;
}

bool LcdKeypad::isDownPressed() {
	return this->value >= 200 && this->value < 400;
}

bool LcdKeypad::isLeftPressed() {
	return this->value >= 400 && this->value < 600;
}

bool LcdKeypad::isSelectPressed() {
	return this->value >= 600 && this->value < 800;
}

bool LcdKeypad::isAnyPressed() {
	return this->value < 800;
}

int LcdKeypad::readValue() {
	this->value = analogRead(A0);
	return this->value;
}

void LcdKeypad::backlightOn() {
	this->blTrunedOn = millis();
	SafeBLon(pin_BL);
}

void LcdKeypad::backlightOff() {
	this->blTrunedOn = 0;
	SafeBLoff(pin_BL);
}

void LcdKeypad::autoToggleBacklight() {
	if (this->isAnyPressed())
	{
		this->backlightOn();
	}
	else if (this->blTrunedOn > 0 && millis() - this->blTrunedOn > backlightTimeout) {
		this->backlightOff();
	}
}

void LcdKeypad::setup() {
	this->lcd->createChar(CharUp, charUp);
	this->lcd->createChar(CharDown, charDown);
	this->lcd->createChar(CharPause, charPause);
	this->lcd->begin(16, 2);
}

void LcdKeypad::setCursor(int x, int y) {
	this->lcd->setCursor(x, y);
}

void LcdKeypad::print(char* value) {
	this->lcd->print(value);
}

void LcdKeypad::print(int value) {
	this->lcd->print(value);
}

void LcdKeypad::print(float value) {
	this->lcd->print(value);
}

void LcdKeypad::printArrowUp(int x, int y) {
	this->setCursor(x, y);
	this->lcd->write(CharUp);
}

void LcdKeypad::printArrowDown(int x, int y) {
	this->setCursor(x, y);
	this->lcd->write(CharDown);
}

void LcdKeypad::printPause(int x, int y) {
	this->setCursor(x, y);
	this->lcd->write(CharPause);
}

void LcdKeypad::printEmpty(int x, int y) {
	this->setCursor(x, y);
	this->lcd->print(' ');
}