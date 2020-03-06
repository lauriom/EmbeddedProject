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
static I2CM_XFER_T  i2cmXferRec;
class PressureSensor {
public:
	PressureSensor();
	~PressureSensor();
	int16_t readSensorData();
	void Readi2c(uint8_t,
			uint8_t *,
			uint16_t ,
			uint8_t *,
			uint16_t);
private:
	uint8_t cmdcode = 0xf1;
	int16_t result;
	uint8_t data[3];
};


#endif /* SRC_RESOURCES_PRESSURESENSOR_H_ */
