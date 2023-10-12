/*
*/

#ifndef  _DEBUG
//#define _DEBUG
#endif // ! _DEBUG

#include "Peltier.h"

//Libraries
#include <LiquidCrystal.h>
#include <DHT_U.h>
#include "LcdKeypad.h"
#include "Peltier.h"

#ifndef DPRINT
#ifdef _DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#endif
#endif

#define  INSIDE_SCREEN 0
#define  OUTSIDE_SCREEN 1
#define  TEMP_SCREEN 2
#define  DELAY_SCREEN 3

#define  INSIDE_DHT 0
#define  OUTSIDE_DHT 1

// functional connections

#define LPWM 2
#define RPWM 3
#define COOLERS 19

#define SCREEN_UPDATE_RATE 300
#define TEMP_MIN_LIMIT 12
#define TEMP_MAX_LIMIT 35

#define TEMP_RANGE 1

//Constants
// Initialize DHT sensor for normal 16mhz Arduino
DHT dhtIn(16, DHT22);
DHT dhtOut(15, DHT22);

LcdKeypad lcd(0);
Peltier peltier(RPWM, LPWM, COOLERS);

int currentScreen = 0;
char* screenTitles[5];
float currentTemp[2];
int currentHum[2];

int tempMin = 24;
int tempMax = 25;

unsigned long lastOutput = 0;
unsigned long lastScreenUpd = 0;
unsigned long delaySecs = 30;

void setup() {

	Serial.begin(9600);
	DPRINTLN("Setup init.");

	//pinMode(11, OUTPUT);
	//digitalWrite(11, HIGH);

	screenTitles[INSIDE_SCREEN] = "    Inside      ";
	screenTitles[OUTSIDE_SCREEN] = "    Outside     ";
	screenTitles[TEMP_SCREEN] = "  Temperature   ";
	screenTitles[DELAY_SCREEN] = "     Delay      ";

	dhtIn.begin();
	dhtOut.begin();
	lcd.setup();
	peltier.setup();

	setScreen(INSIDE_SCREEN);

	DPRINTLN("Setup complete.");
}

void loop() {

	// read temperature and humidity.
	readSensors();

	// read input and process user input.
	processUserInput();

	// turn petier on or off.
	togglePeltier();

	// refresh screen.
	updateScreen(false);

	// execute every 1000ms aprox
	if (millis() - lastOutput > 1000) {
		lastOutput += 1000;

		// code here is executed every 1s.

		// updates timer
		unsigned long prev = delaySecs;
		delaySecs = delaySecs <= 0 ? 0 : delaySecs - 1;

		if (delaySecs == 0 && prev > 0) {
			setScreen(INSIDE_SCREEN);
		}
	}

}

void processUserInput()
{
	int x = lcd.readValue();

	lcd.autoToggleBacklight();

	if (lcd.isRightPressed()) {
		DPRINT("Rigth ");
		setScreen(++currentScreen);
	}
	else if (lcd.isLeftPressed()) {
		DPRINT("Left ");
		setScreen(--currentScreen);
	}
	else if (lcd.isSelectPressed()) {
		DPRINT("Select ");
		setScreen(delaySecs > 0 ? DELAY_SCREEN : INSIDE_SCREEN);
	}
	else if (lcd.isUpPressed()) {
		if (currentScreen == TEMP_SCREEN) {
			tempMin = tempMin >= TEMP_MAX_LIMIT ? TEMP_MAX_LIMIT : tempMin + 1;
			tempMax = tempMin + TEMP_RANGE;

			DPRINT("Up -");
			DPRINT(" Screen ");
			DPRINT(currentScreen);
			DPRINT(" - Temp Min:");
			DPRINT(tempMin);
			DPRINT(" - Temp Max:");
			DPRINTLN(tempMax);
		}

		if (currentScreen == DELAY_SCREEN) {
			long maxDelay = 10 * 60 * 60; //10h
			long increment = 600;// 10min
			delaySecs = delaySecs + increment >= maxDelay ? maxDelay : delaySecs += increment;
		}

		updateScreen(true);
	}
	else if (lcd.isDownPressed()) {
		if (currentScreen == TEMP_SCREEN) {
			tempMin = tempMin <= TEMP_MIN_LIMIT ? TEMP_MIN_LIMIT : tempMin - 1;
			tempMax = tempMin + TEMP_RANGE;

			DPRINT("Down -");
			DPRINT(" Screen ");
			DPRINT(currentScreen);
			DPRINT(" - Temp Min:");
			DPRINT(tempMin);
			DPRINT(" - Temp Max:");
			DPRINTLN(tempMax);
		}

		if (currentScreen == DELAY_SCREEN) {
			delaySecs = delaySecs <= 60 ? 0 : delaySecs - 60;
		}

		updateScreen(true);
	}

	// prevent multi changes for keeping pressed.
	if (lcd.isAnyPressed())
	{
		delay(300);
	}
}

void readSensors()
{
	currentTemp[INSIDE_DHT] = dhtIn.readTemperature();
	currentTemp[OUTSIDE_DHT] = dhtOut.readTemperature();

	currentHum[INSIDE_DHT] = dhtIn.readHumidity();
	currentHum[OUTSIDE_DHT] = dhtOut.readHumidity();
}

void setScreen(int screenNumber) {

	if (screenNumber < INSIDE_SCREEN) {
		screenNumber = DELAY_SCREEN;
	}
	else if (screenNumber > DELAY_SCREEN) {
		screenNumber = INSIDE_SCREEN;
	}

	lcd.setCursor(0, 0);
	lcd.print(screenTitles[screenNumber]);
	currentScreen = screenNumber;

	DPRINT("Changed to Screen ");
	DPRINTLN(screenNumber);

	updateScreen(true);
}

void updateScreen(bool force)
{
	if (force || millis() - lastScreenUpd > 0)
	{
		lastScreenUpd += 300;

		switch (currentScreen)
		{
		case INSIDE_SCREEN:
			printTemp(currentTemp[INSIDE_DHT], currentHum[INSIDE_DHT]);
			break;
		case OUTSIDE_SCREEN:
			printTemp(currentTemp[OUTSIDE_DHT], currentHum[OUTSIDE_DHT]);
			break;
		case TEMP_SCREEN:
			printTemp();
			break;
		case DELAY_SCREEN:
			printDelay();
		default:
			break;
		}

		lcd.setCursor(15, 0);
		char* pstate =
			peltier.isHot() ? "H" :
			peltier.isCold() ? "C" : " ";
		lcd.print(pstate);
	}

	blinkPeltierState();
}

void printDelay()
{
	char buff[16];
	char strHs[2];
	char strMin[2];

	lcd.setCursor(1, 1);

	int hs = 0;
	int min = 0;
	int sec = 0;

	if (delaySecs > 0)
	{
		hs = delaySecs / 60 / 60;
		min = delaySecs / 60 - hs * 60;
		sec = delaySecs - hs * 60 * 60 - min * 60;
	}

	sprintf(buff, "   %02d:%02d:%02d  ", hs, min, sec);

	lcd.print(buff);
}

void printTemp()
{
	char buff[16];

	lcd.setCursor(1, 1);

	sprintf(buff, "      %dc     ", tempMin);

	lcd.print(buff);
}

void printTemp(float temp, int hum)
{
	char buff1[16];
	char buff2[16];
	lcd.setCursor(1, 1);

	sprintf(buff1, "  %sc - %2d%%   ", f(buff2, temp), hum);

	lcd.print(buff1);
}

void togglePeltier()
{
	int state = peltier.getCurrentState();
	if (delaySecs > 0) {
		state = peltier.PELTIER_OFF;
	}
	else if (currentTemp[OUTSIDE_DHT] <= tempMin &&
		currentTemp[INSIDE_DHT] <= tempMin)
	{
		// HOT mode. temperature outside is lower than the minTem
		//			 needs to raise the temp.

		// Turn on when the inner temperature is lower or equal to minTemp 
		state = peltier.PELTIER_HOT;
	}
	else if (
		currentTemp[OUTSIDE_DHT] >= tempMax &&
		currentTemp[INSIDE_DHT] >= tempMax)
	{
		// COLD mode. temperature outside is higher than the maxTem
		//			 needs to lower the temp.

		// only want to turn on when the inner temperature 
		// is higher or equal than maxTemp 
		state = peltier.PELTIER_COLD;
	}
	else if (
		currentTemp[OUTSIDE_DHT] > tempMin && currentTemp[OUTSIDE_DHT] < tempMax || //outside in range
		currentTemp[OUTSIDE_DHT] > tempMax && currentTemp[INSIDE_DHT] <= tempMin || //outside hot inside in bottom
		currentTemp[OUTSIDE_DHT] < tempMin && currentTemp[INSIDE_DHT] >= tempMax)	//outside cold inside in top
	{
		// else when the outside temp is between the min and max temp, then the peltier is no needed.
		//      when the outside temp is above max temp, and inside is below min, shut down peltier.
		//		when the outside temp is below min temp, and inside is above max, shut down peltier.
		state = peltier.PELTIER_OFF;
	}

	// else no change.

	peltier.turn(state);
}


void blinkPeltierState()
{
	unsigned long hsecs = millis() / 500;
	if (hsecs % 3 == 0)
	{
		if (delaySecs > 0)
		{
			lcd.printPause(0, 1);
			return;
		}
		if (peltier.getCurrentState() == peltier.PELTIER_HOT)
		{
			lcd.printArrowUp(0, 1);
			return;
		}
		else if (peltier.getCurrentState() == peltier.PELTIER_COLD)
		{
			lcd.printArrowDown(0, 1);
			return;
		}
	}

	lcd.printEmpty(0, 1);
}

char* f(char* buffer, float value)
{
	int integerPart = value;
	int decimalPart = (value - integerPart) * 10;

	sprintf(buffer, "%d.%d", integerPart, decimalPart);;

	return buffer;
}
