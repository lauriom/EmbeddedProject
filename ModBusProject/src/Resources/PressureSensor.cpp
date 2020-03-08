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
	uint8_t cmdcode = 0xf1; //not needed as private members if data isn't saved
	uint8_t data[3];
	Readi2c(0x40, &cmdcode, 1, data, 3);
	return (data[0] << 8) | data[1];;
}

int PressureSensor::readPressureInPa() {
	int16_t rawData = readSensorData();

	return rawData / 240; //rounded. Float precision not needed. Scale factor 240. Max signed 16bit 32767 / 240 = 136 Pa theoretically

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

