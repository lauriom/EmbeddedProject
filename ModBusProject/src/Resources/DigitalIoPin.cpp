/*
 * DigitalIoPin.cpp
 *
 *  Created on: 26 Feb 2020
 *      Author: Lauri
 */

#include "DigitalIoPin.h"

DigitalIoPin::DigitalIoPin(int Port, int Pin, bool Input, bool Pullup, bool Invert)
:port(Port),pin(Pin),invert(Invert){
	if(Input){ // sets settings
		Chip_IOCON_PinMuxSet (LPC_IOCON,port,pin,(Pullup ? IOCON_MODE_PULLUP : 0x0) | IOCON_DIGMODE_EN | (Invert ? IOCON_INV_EN : 0x0));
		Chip_GPIO_SetPinDIRInput(LPC_GPIO,port,pin); // sets as input
	}else{
		Chip_IOCON_PinMuxSet (LPC_IOCON,port,pin,(Pullup ? IOCON_MODE_PULLUP : 0x0) | IOCON_DIGMODE_EN);
		Chip_GPIO_SetPinDIROutput(LPC_GPIO,port,pin); // sets as output
	}
};

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}
/**
 * @brief flips value of pin b = !b
 UNUSED
void DigitalIoPin::flipOutput(){
	LPC_GPIO->B[port][pin] = !LPC_GPIO->B[port][pin];
}
 */
/**
 * @brief Sets bool to pin. Will invert if invert set to true in constructor
 */
void DigitalIoPin::write(bool b){
	Chip_GPIO_SetPinState(LPC_GPIO,port,pin, (invert) ? !b : b);
}
/**
 * @brief Returns volatile value of pin
 */
volatile bool DigitalIoPin::read(){
	return Chip_GPIO_GetPinState(LPC_GPIO,port,pin);
}
/**
 * @brief Same as write
 */
inline void DigitalIoPin::operator=(const bool b){
	write(b);
}
/**
 * @brief Same as read
 */
inline DigitalIoPin::operator bool(){
	return read();
}

