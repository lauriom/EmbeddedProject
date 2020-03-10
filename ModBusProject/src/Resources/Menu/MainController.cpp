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
	paResult = ps->readPressureInPa();
}

MainController::~MainController() {
	// TODO Auto-generated destructor stub
}

void MainController::updateMenu(){
	menuEventHandler();
	std::string str;
	lcd->setCursor(7,0); // leaves 1 space empty
	lcd->print(errorState ? "Error" :"OK   ");



	str = std::to_string(paResult);
	lcd->setCursor(1,0); // leaves 1 space empty
	lcd->print(str);
	lcd->print("  ");
	lcd->setCursor(4,0);
	lcd->print(" P");

	lcd->setCursor(5,1);
	if(autoMode){
		lcd->print("P ");
		lcd->print("Auto");
		lcd->setCursor(1,1);
		str = std::to_string(targetPressure);
		lcd->print(str);
		lcd->print(" ");

	}else{
		str = std::to_string(targetSpeedInPercent);
		lcd->print("% ");
		lcd->print("Manual");
		lcd->setCursor(1,1);
		str = std::to_string(targetSpeedInPercent);
		lcd->print(str);
		lcd->print(" ");

	}
}

void MainController::menuEventHandler(){
	int event;
	while((event = buffer->get()) != 0){
		switch(event){
		case 1:
			if (autoMode){
				++targetPressure;
			}else{
				++targetSpeedInPercent;
			}
			//up
			break;
		case 2:
			autoMode = !autoMode;
			break;
		case 3:
			if (autoMode){
				--targetPressure;
			}else{
				--targetSpeedInPercent;
			}
			break;
		case 0:
		default:
			break;
		}
	}
	int pressureDiff = abs(targetPressure - paResult);
	step = remapRange(pressureDiff, PRES_DIFF_MIN_STEP, PRES_DIFF_MAX_STEP, FAN_SPEED_MIN_STEP, FAN_SPEED_MAX_STEP);
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
	static int noPressureDiffCounter = 0; // counts cycles pressure is not reached

	paResult = ps->readPressureInPa();

	errorState = false;
	if (autoMode) {
		int pressureDiff = abs(targetPressure - paResult);

		if(pressureDiff > 10){
			if (noPressureDiffCounter++ > 1000){ //
				errorState = true;
			}
		}else{
			noPressureDiffCounter = 0;
		}

		//DEBUGOUT("Auto mode. Pa result: %d\n", paResult);

		if (pressureDiff > (0.25 * PRES_MAX)) {
			step = remapRange(pressureDiff, PRES_DIFF_MIN_STEP, PRES_DIFF_MAX_STEP, FAN_SPEED_MIN_STEP, FAN_SPEED_MAX_STEP);
		}
		else {
			step = FAN_SPEED_STEP;
		}
		//DEBUGOUT("Step is %d\n", step);

		if (paResult < targetPressure) {
			fanFreq += step;
		}

		if (paResult > targetPressure) {
			fanFreq -= step;
		}

		fanFreq = clamp(fanFreq, MIN_FAN_SPEED, MAX_FAN_SPEED); //keep within range
		fan->setFrequency(fanFreq);

	}

	else { //manual mode

		int targetFrequency = remapRange(targetSpeedInPercent, 0, 100, MIN_FAN_SPEED, MAX_FAN_SPEED);

		fan->setFrequency(targetFrequency);

		//DEBUGOUT("Manual mode. Pa result: %d\n", paResult);

	}
}

