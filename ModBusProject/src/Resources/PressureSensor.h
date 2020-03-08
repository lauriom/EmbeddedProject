/*
 * PressureSensor.h
 *
 *  Created on: 6 Mar 2020
 *      Author: Taneli
 */

#ifndef SRC_RESOURCES_PRESSURESENSOR_H_
#define SRC_RESOURCES_PRESSURESENSOR_H_

#include <cstdint>
#include "board.h"

class PressureSensor {
public:
	PressureSensor();
	~PressureSensor();
	int16_t readSensorData();
	int readPressureInPa();
	void Readi2c(uint8_t,
			uint8_t *,
			uint16_t ,
			uint8_t *,
			uint16_t);
};


#endif /* SRC_RESOURCES_PRESSURESENSOR_H_ */
