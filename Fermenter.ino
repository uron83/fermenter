/*
*/
#include "Peltier.h"
#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.

//Libraries
#include <LiquidCrystal.h>
#include <DHT.h>;
#include "LcdKeypad.h"
#include "Peltier.h"

#ifndef DPRINT
#ifdef DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#endif
#endif

#define  INSIDE_SCREEN 0
#define  OUTSIDE_SCREEN 1
#define  MIN_SCREEN 2
#define  MAX_SCREEN 3

#define  INSIDE_DHT 0
#define  OUTSIDE_DHT 1

#define HG7881_B_IA 2 // D10 --> Motor B Input A --> MOTOR B +
#define HG7881_B_IB 3 // D11 --> Motor B Input B --> MOTOR B -

// functional connections

#define LPWM 2
#define RPWM 3
#define COOLERS 19

#define SCREEN_UPDATE_RATE 300
#define TEMP_MIN_LIMIT 12
#define TEMP_MAX_LIMIT 35


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

void setup() {

	Serial.begin(9600);
	DPRINTLN("Setup init.");

	//pinMode(11, OUTPUT);
	//digitalWrite(11, HIGH);

	screenTitles[INSIDE_SCREEN] = "     Inside     ";
	screenTitles[OUTSIDE_SCREEN] = "     Outside    ";
	screenTitles[MIN_SCREEN] = "    Min Temp    ";
	screenTitles[MAX_SCREEN] = "    Max Temp    ";

	dhtIn.begin();
	dhtOut.begin();
	lcd.setup();
	peltier.setup();

	setScreen(INSIDE_SCREEN);

	DPRINTLN("Setup complete.");
}

void loop() {

	int x = lcd.readValue();
	currentTemp[INSIDE_DHT] = dhtIn.readTemperature();
	currentTemp[OUTSIDE_DHT] = dhtOut.readTemperature();

	currentHum[INSIDE_DHT] = dhtIn.readHumidity();
	currentHum[OUTSIDE_DHT] = dhtOut.readHumidity();

	lcd.autoToggleBacklight();

	if (lcd.isRightPressed()) {
		DPRINT("Rigth");
		setScreen(++currentScreen);
	}
	else if (lcd.isLeftPressed()) {
		DPRINT("Left");
		setScreen(--currentScreen);
	}
	else if (lcd.isSelectPressed()) {
		DPRINT("Select");
		setScreen(0);
	}
	else if (lcd.isUpPressed()) {
		if (currentScreen == MIN_SCREEN) {
			tempMin = tempMin >= TEMP_MAX_LIMIT - 1 ? TEMP_MAX_LIMIT - 1 : tempMin + 1;

			if (tempMin >= tempMax)
			{
				tempMax = tempMin + 1;
			}
		}
		else if (currentScreen == MAX_SCREEN) {
			tempMax = tempMax >= TEMP_MAX_LIMIT ? TEMP_MAX_LIMIT : tempMax + 1;
		}

		DPRINT("Up -");
		DPRINT(" Screen ");
		DPRINT(currentScreen);
		DPRINT(" - Temp Min:");
		DPRINT(tempMin);
		DPRINT(" - Temp Max:");
		DPRINTLN(tempMax);

		updateScreen(true);
	}
	else if (lcd.isDownPressed()) {
		if (currentScreen == MIN_SCREEN) {
			tempMin = tempMin <= TEMP_MIN_LIMIT ? TEMP_MIN_LIMIT : tempMin - 1;
		}
		else if (currentScreen == MAX_SCREEN) {
			tempMax = tempMax <= TEMP_MIN_LIMIT + 1 ? TEMP_MIN_LIMIT + 1 : tempMax - 1;

			if (tempMin >= tempMax)
			{
				tempMin = tempMax - 1;
			}
		}

		DPRINT("Down -");
		DPRINT(" Screen ");
		DPRINT(currentScreen);
		DPRINT(" - Temp Min:");
		DPRINT(tempMin);
		DPRINT(" - Temp Max:");
		DPRINTLN(tempMax);

		updateScreen(true);
	}

	// prevent multi changes for keeping pressed.
	if (lcd.isAnyPressed())
	{
		delay(300);
	}

	togglePeltier();

	updateScreen(false);

	// print every 1000ms aprox
	if (millis() - lastOutput > 1000) {
		lastOutput += 1000;
		char buff[8];

		//DPRINTLN(millis());
		//DPRINT("Temp in: ");
		//DPRINT(f(buff, currentTemp[0]));
		//DPRINT("c out: ");
		//DPRINT(f(buff, currentTemp[1]));
		//DPRINTLN("c");


		//DPRINT("Hum in: ");
		//DPRINT(currentHum[0]);
		//DPRINT("% out: ");
		//DPRINT(currentHum[1]);
		//DPRINTLN("%");
	}

}

void setScreen(int screenNumber) {

	if (screenNumber < INSIDE_SCREEN) {
		screenNumber = MAX_SCREEN;
	}
	else if (screenNumber > MAX_SCREEN) {
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
	if (force || millis() - lastScreenUpd > 300)
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
		case MIN_SCREEN:
		case MAX_SCREEN:
			printRange();
			break;
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

void printRange()
{
	char buff[16];

	lcd.setCursor(1, 1);

	sprintf(buff, "  %dc -- %dc", tempMin, tempMax);

	lcd.print(buff);
}

void printTemp(float temp, int hum)
{
	char buff1[16];
	char buff2[16];
	lcd.setCursor(1, 1);

	sprintf(buff1, " %sc - %2d%%", f(buff2, temp), hum);

	lcd.print(buff1);
}

void togglePeltier()
{
	int state = peltier.getCurrentState();
	if (currentTemp[OUTSIDE_DHT] <= tempMin &&
		currentTemp[INSIDE_DHT] <= tempMin)
	{
		// HOT mode. temperature outside is lower than the minTem
		//			 needs to keep the temp higher.

		// only want to turn on when the inner temperature 
		// is lower or equal to minTemp 
		state = peltier.PELTIER_HOT;
	}
	else if (currentTemp[OUTSIDE_DHT] >= tempMax &&
		currentTemp[INSIDE_DHT] >= tempMax)
	{
		// COLD mode. temperature outside is higher than the maxTem
		//			 needs to lower the temp.

		// only want to turn on when the inner temperature 
		// is higher or equal than maxTemp 
		state = peltier.PELTIER_COLD;
	}
	else if (currentTemp[OUTSIDE_DHT] > tempMin && currentTemp[OUTSIDE_DHT] < tempMax || //outside in range
		currentTemp[OUTSIDE_DHT] > tempMax && currentTemp[INSIDE_DHT] <= tempMin || //outside hot inside in bottom
		currentTemp[OUTSIDE_DHT] < tempMin && currentTemp[INSIDE_DHT] >= tempMax)	//outside cold insite in top
	{
		// else is when the outside temp is between the min and max temp, then the peltier is no needed.
		state = peltier.PELTIER_OFF;
	}

	peltier.turn(state);
}


void blinkPeltierState()
{
	unsigned long secs = millis() / 500;
	if (secs % 3 == 0)
	{
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


// Format string up to 512 chars
void sprintln(char* msg, ...) {
	/* Declare a va_list type variable */
	va_list argList;
	char buffer[512] = { 0 };

	/* Initialise the va_list variable with the ... after fmt */
	va_start(argList, msg);

	/* format */
	vsprintf(buffer, msg, argList);

	Serial.println(buffer);

	/* Clean up the va_list */
	va_end(argList);
}

char* f(char* buffer, float value)
{
	int integerPart = value;
	int decimalPart = (value - integerPart) * 10;

	sprintf(buffer, "%d.%d", integerPart, decimalPart);;

	return buffer;
}
