/*
 * MainController.cpp
 *
 *  Created on: 6 Mar 2020
 *      Authors: Lauri, Monika, Taneli
 */

#include "MainController.h"

const int STEP = 5; //for UI, changing percent and pressure


MainController::MainController(LiquidCrystal* screen,PressureSensor* psen,Fan* f,RINGBUFF_T* buf):
lcd(screen), ps(psen), fan(f), buffer(buf) {

	fan->startFan();
	paResult = ps->readPressureInPa();
}

MainController::~MainController() {
}

void MainController::updateMenu() {

	menuEventHandler();
	std::string str;
	lcd->setCursor(8,0); // leaves 1 space empty
	lcd->print(errorState ? "Error" :"     ");

	str = std::to_string(paResult);
	lcd->setCursor(1,0); // leaves 1 space empty
	lcd->print(str);
	lcd->print("  ");
	lcd->setCursor(4,0);
	lcd->print(" Pa");
	lcd->setCursor(5,1);

	if(autoMode) {

		lcd->print("Pa ");
		lcd->print("Auto  ");
		lcd->setCursor(1, 1);
		str = std::to_string(targetPressure);
		lcd->print(str);
		lcd->print(" ");
	}

	else {

		lcd->print("% ");
		lcd->print("Manual");
		lcd->setCursor(1, 1);
		str = std::to_string(targetSpeedInPercent);
		lcd->print(str);
		lcd->print(" ");

	}
}

void MainController::menuEventHandler() {
	int event = 0;

	while(RingBuffer_Pop(buffer,&event)) {

		switch(event) {

		case 1: //increasing a value by STEP
			if (autoMode) {

				targetPressure += STEP;
			}

			else {

				targetSpeedInPercent += STEP;
			}

			break;

		case 2: //toggling modes
			autoMode = !autoMode;
			break;

		case 3: //decreasing a value by STEP
			if (autoMode) {

				targetPressure -= STEP;
			}

			else {

				targetSpeedInPercent -= STEP;
			}

			break;

		case 0:
		default:
			break;
		}
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

void MainController::run() {

	paResult = ps->readPressureInPa();
	errorState = false;

	if (autoMode) {

		int pressureDiff = abs(targetPressure - paResult);

		if(pressureDiff > ErrorStateTriggerDist) {

			if (noPressureDiffCounter++ > 100) {

				errorState = true;// if not near desired pressure in 100 cycles, trigger error
			}
		}

		else {

			noPressureDiffCounter = 0;
		}

		int step = remapRange(pressureDiff, PRES_DIFF_MIN_STEP, PRES_DIFF_MAX_STEP, FAN_SPEED_MIN_STEP, FAN_SPEED_MAX_STEP);

		if (paResult < targetPressure) {
			fanFreq += step;
		}

		if (paResult > targetPressure) {
			fanFreq -= step;
		}

		fan->setFrequency(fanFreq);
	}

	else { //manual mode

		int targetFrequency = remapRange(targetSpeedInPercent, 0, 100, MIN_FAN_SPEED, MAX_FAN_SPEED);
		fan->setFrequency(targetFrequency);

	}
}

