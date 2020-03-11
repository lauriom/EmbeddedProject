/*
 * MenuController.h
 *
 *  Created on: 6 Mar 2020
 *      Authors: Lauri, Monika, Taneli
 */

#ifndef SRC_RESOURCES_MENU_MENUCONTROLLER_H_
#define SRC_RESOURCES_MENU_MENUCONTROLLER_H_

#include <string>

#include "Menu/LiquidCrystal.h"
#include "Menu/LimitedInt.h"

#include "ring_buffer.h"

#include "Fan.h"
#include "PressureSensor.h"

#define ErrorStateTriggerDist 5

const int MAX_FAN_SPEED = 20000;
const int MIN_FAN_SPEED = 0;
const int FAN_SPEED_STEP = 200; //increment/decrement by this speed at a time when controlling the fan
const int FAN_SPEED_MIN_STEP = 40; //by 1 Hz
const int FAN_SPEED_MAX_STEP = 5000; //max fan freq is 500 Hz. Max step could be 250 Hz (half)
const int PRES_DIFF_MIN_STEP = 1; //pressure difference to use min fan speed step at
const int PRES_DIFF_MAX_STEP = 60; //pressure diff when max fan speed step is used
const int PRES_MAX = 120; // pressure upper range
const int PRES_MIN = 0; // pressure lower range
/*
 * Controller for program, reads sensor runs fan and updates Ui
 */
class MainController {
public:
	MainController(LiquidCrystal*,PressureSensor*,Fan*,RINGBUFF_T*);
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
	RINGBUFF_T *buffer;

	// Values of sensors
	LimitedInt fanFreq = LimitedInt(0,MIN_FAN_SPEED, MAX_FAN_SPEED); // frequency of fan
	int paResult; // pressure in pascals

	bool autoMode = true; // controls state
	bool errorState = false; // true if desired pressure wont be reached
	int noPressureDiffCounter = 0; // counter for triggering errorstate.

	LimitedInt targetPressure = LimitedInt(20,0,120); // target pascal pressure, range 0-120
	LimitedInt targetSpeedInPercent = LimitedInt(30,0,100);// Manual fan speed in 0-100%
};

#endif /* SRC_RESOURCES_MENU_MENUCONTROLLER_H_ */
