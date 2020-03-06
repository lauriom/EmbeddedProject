/*
 * Fan.h
 *
 *  Created on: 6 Mar 2020
 *      Author: Taneli
 */

#ifndef SRC_RESOURCES_FAN_H_
#define SRC_RESOURCES_FAN_H_
#include <cstdint>
#include "board.h"
#include "Modbus/ModbusMaster.h"
#include "Modbus/ModbusRegister.h"

class Fan {
public:
	Fan(ModbusMaster *);
	~Fan();
	bool setFrequency(uint16_t);
	uint16_t getFrequency();
	void startFan();
private:
	ModbusMaster n;
	ModbusRegister control;
	ModbusRegister frequency;
	ModbusRegister Status;
	ModbusRegister outputFrequency;

};


#endif /* SRC_RESOURCES_FAN_H_ */
