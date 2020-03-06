/*
 * Fan.cpp
 *
 *  Created on: 6 Mar 2020
 *      Author: Lauri
 */

#include "Fan.h"

Fan::Fan(ModbusMaster *node) : control(node, 0), frequency(node, 1), Status(node, 3), outputFrequency(node, 102){
	n = *node;

}

Fan::~Fan() {
	// TODO Auto-generated destructor stub
}
uint16_t Fan::getFrequency() {
	return outputFrequency;
}

void Fan::startFan() {

	control = 0x0406; // prepare for starting

	//Sleep(1000); // give converter some time to set up

	control = 0x047F; // set drive to start mode

	printf("Status=%04X\n", (int)Status); // for debugging
}
bool Fan::setFrequency(uint16_t freq)
{
	int result;
	int ctr;
	bool atSetpoint;
	const int delay = 10;


	frequency = freq; // set motor frequency

	printf("Set freq = %d\n", freq/40); // for debugging

	// wait until we reach set point or timeout occurs
	ctr = 0;
	atSetpoint = false;
	do {
		for(int i = 0 ; i < 7200000; i++);
		result = Status;
		// check if we are at setpoint
		if (result >= 0 && (result & 0x0100)) atSetpoint = true;
		ctr++;
	} while(ctr < 20 && !atSetpoint);

	printf("Elapsed: %d\n", ctr * delay); // for debugging

	return atSetpoint;
}


