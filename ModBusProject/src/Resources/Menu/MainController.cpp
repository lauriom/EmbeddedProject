/*
 * MainController.cpp
 *
 *  Created on: 6 Mar 2020
 *      Authors: Lauri, Monika, Taneli
 */

#include "MainController.h"


MainController::MainController(LiquidCrystal* screen,PressureSensor* psen,Fan* f,RingBuffer* buf):
lcd(screen),ps(psen),fan(f),buffer(buf) {
	fan->startFan(); // initialies fan
}

MainController::~MainController() {
	// TODO Auto-generated destructor stub
}

void MainController::updateMenu(){
	std::string row1;
	row1.append(std::to_string(paResult));
	lcd->setCursor(1,0); // leaves 1 space empty
	lcd->print(row1);
}

void MainController::menuEventHandler(int event){


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
/*
 * to clamp values to ranges
 */
int MainController::clamp(int val, int min, int max) {

	if (val < min) return min;
	if (val > max) return max;
	return val;

}
/*
 * input range to output range
 */
int MainController::remapRange (int val, int iMin, int iMax, int oMin, int oMax) {

	val = clamp(val, iMin, iMax); //restrict value to input range
	float valFraction = float(val - iMin) / float(iMax - iMin); //result between 0 and 1

	return int((oMax - oMin) * valFraction) + oMin; //find where the float would be on the output range
}
void MainController::run(){

	if (autoMode) {

		paResult = ps->readPressureInPa();

		printf("Auto mode. Pa result: %d\n", paResult);

		if (paResult != targetPressure) {

			int pressureDiff = abs(targetPressure - paResult);
			int step = remapRange(pressureDiff, PRES_DIFF_MIN_STEP, PRES_DIFF_MAX_STEP, FAN_SPEED_MIN_STEP, FAN_SPEED_MAX_STEP);
			printf("Step is %d\n", step);

			if (paResult < targetPressure) { //adjusting pressure could be improved
				fanFreq += step;
			}

			if (paResult > targetPressure) {
				fanFreq -= step;
			}

			fanFreq = clamp(fanFreq, MIN_FAN_SPEED, MAX_FAN_SPEED); //keep within range
		}

		fan->setFrequency(fanFreq);
	}

	else { //manual mode

		int targetFrequency = remapRange(targetSpeedInPercent, 0, 100, MIN_FAN_SPEED, MAX_FAN_SPEED);

		fan->setFrequency(targetFrequency);
		printf("Manual mode. Pa result: %d\n", paResult);

	}
	updateMenu();
}

