// 
// 
// 
#ifndef  _DEBUG
//#define _DEBUG
#endif // ! _DEBUG

#include "Peltier.h"

#ifndef DPRINT
#ifdef _DEBUG    //Macros are usually in all capital letters.
#define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
#define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
#define DPRINT(...)     //now defines a blank line
#define DPRINTLN(...)   //now defines a blank line
#endif // !DPRINT
#endif


Peltier::Peltier(unsigned int rpwm, unsigned int lpwm, unsigned int coolers)
{
	this->rpwm = rpwm;
	this->lpwm = lpwm;
	this->coolers = coolers;
}

void Peltier::setup()
{
	pinMode(this->rpwm, OUTPUT);
	pinMode(this->lpwm, OUTPUT);
	pinMode(this->coolers, OUTPUT);

	digitalWrite(this->rpwm, LOW);
	digitalWrite(this->lpwm, LOW);
	digitalWrite(this->coolers, LOW);

}

// Tries to set the Peltier state, prevent hot to cold and vice-versa sudden changes.
// use getCurrentState to determine whether the state was correclty set.
void Peltier::turn(unsigned short state)
{
	if (this->currentState != state)
	{

		int previousState = this->currentState;

		if (state == PELTIER_HOT)
		{
			if (this->isCold()) {
				this->currentState = PELTIER_OFF;
			}
			else {
				DPRINTLN("Peltier canged state to HOT");
				this->currentState = state;
				this->physicalState = state;
				this->millisSincePhysicalState = millis();
			}
		}
		else if (state == PELTIER_COLD)
		{
			if (this->isHot()) {
				this->currentState = PELTIER_OFF;
			}
			else {
				DPRINTLN("Peltier canged state to COLD");
				this->currentState = state;
				this->physicalState = state;
				this->millisSincePhysicalState = millis();
			}
		}
		else
		{
			DPRINTLN("Peltier canged state to OFF");
			this->currentState = PELTIER_OFF;
			this->millisSincePhysicalState = millis();
		}
	}

	this->updatePhysicalState();
}

// returns a value indicating wether the the peliter is Cold. 
// Retains the value after a period of time after it is turned off.
bool Peltier::isCold() 
{
	long diff = millis() - this->millisSincePhysicalState;
	return
		this->currentState == PELTIER_COLD && diff >= CONSIDER_PHYSICAL_STATE ||
		this->currentState != PELTIER_COLD &&
		this->physicalState == PELTIER_COLD &&	
		diff <= RETAIN_PHYSICAL_STATE;
}

// returns a value indicating wether the the peliter is Hot. 
// Retains the value after a period of time after it is turned off.
bool Peltier::isHot()
{
	long diff = millis() - this->millisSincePhysicalState;
	return
		this->currentState == PELTIER_HOT && diff >= CONSIDER_PHYSICAL_STATE ||
		this->currentState != PELTIER_HOT &&
		this->physicalState == PELTIER_HOT &&
		diff <= RETAIN_PHYSICAL_STATE;
}

unsigned short Peltier::getCurrentState() {
	return this->currentState;
}

// private method to change turn hot, cold and off the peltier. 
// sudden changes are handled by public method turn().
void Peltier::updatePhysicalState()
{
	if (this->currentState == PELTIER_HOT)
	{
		digitalWrite(this->lpwm, LOW);
		digitalWrite(this->rpwm, HIGH);
		digitalWrite(this->coolers, HIGH);
	}
	else if (this->currentState == PELTIER_COLD)
	{
		digitalWrite(this->lpwm, HIGH);
		digitalWrite(this->rpwm, LOW);
		digitalWrite(this->coolers, HIGH);
	}
	else
	{
		digitalWrite(this->lpwm, LOW);
		digitalWrite(this->rpwm, LOW);
		digitalWrite(this->coolers, LOW);
	}
}

