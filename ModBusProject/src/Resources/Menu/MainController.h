/*
 * MenuController.h
 *
 *  Created on: 6 Mar 2020
 *      Author: Lauri
 */

#ifndef SRC_RESOURCES_MENU_MENUCONTROLLER_H_
#define SRC_RESOURCES_MENU_MENUCONTROLLER_H_

#include <string>

#include "LiquidCrystal.h"
#include "../RingBuffer.h"

#define ButtonRight 1
#define ButtonMid 2
#define ButtonLeft 3
#define ButtonTimeout 4

class MainController {
public:
	MainController(LiquidCrystal*);
	virtual ~MainController();
	void eventHandler(int);
private:
	LiquidCrystal *lcd;

	bool autoMode = true;
};

#endif /* SRC_RESOURCES_MENU_MENUCONTROLLER_H_ */
