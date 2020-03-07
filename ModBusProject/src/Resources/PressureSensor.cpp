/*
 * PressureSensor.cpp
 *
 *  Created on: 6 Mar 2020
 *      Author: Taneli
 */

#include "PressureSensor.h"

PressureSensor::PressureSensor() {
	// TODO Auto-generated constructor stub


}

PressureSensor::~PressureSensor() {
	// TODO Auto-generated destructor stub
}

int16_t PressureSensor::readSensorData() {
	Readi2c(0x40, &cmdcode, 1, data, 3);
	result = (data[0] << 8);
	result += data[1];
	return result;
}
void PressureSensor::Readi2c(uint8_t devAddr,
		uint8_t *txBuffPtr,
		uint16_t txSize,
		uint8_t *rxBuffPtr,
		uint16_t rxSize){
	I2CM_XFER_T  i2cmXferRec; //not used in other functions
	i2cmXferRec.slaveAddr = devAddr;
	i2cmXferRec.status = 0;
	i2cmXferRec.txSz = txSize;
	i2cmXferRec.rxSz = rxSize;
	i2cmXferRec.txBuff = txBuffPtr;
	i2cmXferRec.rxBuff = rxBuffPtr;

	Chip_I2CM_XferBlocking(LPC_I2C0, &i2cmXferRec);
}

