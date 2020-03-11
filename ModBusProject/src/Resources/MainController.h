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
#include "ring_buffer.h"
#include "Fan.h"
#include "PressureSensor.h"


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
	int clamp(int val, int min, int max); //clamps values to a range
	int remapRange (int val, int iMin, int iMax, int oMin, int oMax); //remaps values from one range onto another

	// Components of program
	LiquidCrystal *lcd;
	PressureSensor *ps;
	Fan *fan;
	RINGBUFF_T *buffer;

	int paResult; // pressure in pascals
	bool autoMode = true; // controls state
	bool errorState = false; // true if desired pressure wont be reached
	int noPressureDiffCounter = 0; // counter for triggering errorstate.
	int fanFreq = 0;
	int targetPressure = 20; //starting value for auto mode
	int targetSpeedInPercent = 30; //starting value for manual mode

};

#endif /* SRC_RESOURCES_MENU_MENUCONTROLLER_H_ */
