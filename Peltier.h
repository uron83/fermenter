// Peltier.h

#ifndef _PELTIER_h
#define _PELTIER_h

#include "arduino.h"

class Peltier {
public:
	const unsigned short PELTIER_OFF = 0;
	const unsigned short PELTIER_HOT = 1;
	const unsigned short PELTIER_COLD = 2;
	Peltier(unsigned int rpwm, unsigned int lpwm, unsigned int coolers);
	void turn(unsigned short state);
	bool isHot();
	bool isCold();
	void setup();
	unsigned short getCurrentState();
private:
	const unsigned int RETAIN_PHYSICAL_STATE = 10000;
	const unsigned int CONSIDER_PHYSICAL_STATE = 5000;
	unsigned int rpwm;
	unsigned int lpwm;
	unsigned int coolers;
	unsigned short physicalState;
	unsigned short currentState;
	unsigned long millisSincePhysicalState;
	void updatePhysicalState();
};
#endif