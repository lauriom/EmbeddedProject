/*
 * MenuController.cpp
 *
 *  Created on: 6 Mar 2020
 *      Author: Lauri
 */

#include "MainController.h"


MainController::MainController(LiquidCrystal* screen): lcd(screen) {
	std::string row1 = (" xxxxx   mode");
	lcd->print(row1);
}

MainController::~MainController() {
	// TODO Auto-generated destructor stub
}

void MainController::eventHandler(int event){
	std::string row2 = "[yyyyy] [";
	row2.append(autoMode ? "Auto" : "Mann");
	lcd->setCursor(0,1);
	lcd->print(row2);

	switch(event){
	case 1:
		//up
		break;
	case 2:
		//mid
		break;
	case 3:
		// down
		break;
	case 4:
		//turn off screen
		break;
	case 0:
	default:
		break;
	}
}

