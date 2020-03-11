/*
 * Watchdog.h
 *
 *  Created on: 8 Mar 2020
 *      Author: Monika
 */

#ifndef SRC_RESOURCES_WATCHDOG_H_
#define SRC_RESOURCES_WATCHDOG_H_

class Watchdog {
public:
	Watchdog(int resetTimeSec);
	~Watchdog();
	void feed();

};

#endif /* SRC_RESOURCES_WATCHDOG_H_ */
