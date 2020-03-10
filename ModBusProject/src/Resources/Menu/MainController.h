/*
 * MenuController.h
 *
 *  Created on: 6 Mar 2020
 *      Authors: Lauri, Monika, Taneli
 */

#ifndef SRC_RESOURCES_MENU_MENUCONTROLLER_H_
#define SRC_RESOURCES_MENU_MENUCONTROLLER_H_

#include <string>

#include "LiquidCrystal.h"
#include "RingBuffer.h"
#include "LimitedInt.h"
#include "../Fan.h"
#include "../PressureSensor.h"


#define ButtonRight 1
#define ButtonMid 2
#define ButtonLeft 3
#define ButtonTimeout 4

const int MAX_FAN_SPEED = 20000;
const int MIN_FAN_SPEED = 2000;
const int FAN_SPEED_STEP = 200; //increment/decrement by this speed at a time when controlling the fan
const int FAN_SPEED_MIN_STEP = 40; //by 1 Hz
const int FAN_SPEED_MAX_STEP = 5000; //max fan freq is 500 Hz. Max step could be 250 Hz (half)
const int PRES_DIFF_MIN_STEP = 1; //pressure difference to use min fan speed step at
const int PRES_DIFF_MAX_STEP = 60; //pressure diff when max fan speed step is used
const int PRES_MAX = 120; // pressure upper range
const int PRES_MIN = 0; // pressure lower range
/*
 * Controller for program, reads sensor runs fan and updates ui
 */
class MainController {
public:
	MainController(LiquidCrystal*,PressureSensor*,Fan*,RingBuffer*);
	virtual ~MainController();
	void run();
	void updateMenu();
private: // updates menu values
	void menuEventHandler(); // updates on user input
	int clamp(int val, int min, int max);
	int remapRange (int val, int iMin, int iMax, int oMin, int oMax);
	// Components of program
	LiquidCrystal *lcd;
	PressureSensor *ps;
	Fan *fan;
	RingBuffer *buffer;

	// Values of sensors
	bool autoMode = true; // controls mode
	bool errorState = false; // true if desired pressure wont be reached
	int fanFreq = 0; // frequency of fan
	int paResult; // pressure in pascals

	int step = 200; // initial size of adjust for fanspeed
	int targetPressure = 20;
	int targetSpeedInPercent = 50;
	//LimitedInt targetPressure(20,0,120); // target pascal pressure, range 0-120
	//LimitedInt targetSpeedInPercent(30,0,100);// Manual fan speed in 0-100%



};

#endif /* SRC_RESOURCES_MENU_MENUCONTROLLER_H_ */
