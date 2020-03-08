/*
 * Watchdog.cpp
 *
 *  Created on: 8 Mar 2020
 *      Author: Monika
 */

#include "Watchdog.h"
#include "chip.h"

Watchdog::Watchdog(int resetTimeSec) {

	uint32_t wdtFreq;
	Chip_SYSCTL_PowerUp(SYSCTL_POWERDOWN_WDTOSC_PD);
	wdtFreq = Chip_Clock_GetWDTOSCRate() / 4;
	Chip_WWDT_Init(LPC_WWDT); //Initialize watchdog timer
	Chip_WWDT_SetTimeOut(LPC_WWDT, wdtFreq * resetTimeSec); // Feed time passed in
	Chip_WWDT_SetOption(LPC_WWDT, WWDT_WDMOD_WDRESET); //Reset on timeout
	Chip_WWDT_ClearStatusFlag(LPC_WWDT, WWDT_WDMOD_WDTOF); //Reset the flag
	Chip_WWDT_Start(LPC_WWDT); //Start the watchdog

}

Watchdog::~Watchdog() {

	Chip_WWDT_DeInit(LPC_WWDT); //Stop watchdog timer

}

void Watchdog::feed() {

	Chip_WWDT_Feed(LPC_WWDT);
}


