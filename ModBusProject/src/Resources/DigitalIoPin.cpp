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
		LPC_IOCON->PIO[port][pin] = ((Pullup ? IOCON_MODE_PULLUP : 0x0) | IOCON_DIGMODE_EN | (Invert ? IOCON_INV_EN : 0x0));
		LPC_GPIO->DIR[port] &= ~(1UL << pin); // sets as input
	}else{
		LPC_IOCON->PIO[port][pin] = (IOCON_MODE_INACT | IOCON_DIGMODE_EN);
		LPC_GPIO->DIR[port] |= 1UL << pin; // sets as output
	}
};

DigitalIoPin::~DigitalIoPin() {
	// TODO Auto-generated destructor stub
}
/**
 * @brief flips value of pin b = !b
 */
void DigitalIoPin::flipOutput(){
	LPC_GPIO->B[port][pin] = !LPC_GPIO->B[port][pin];
}
/**
 * @brief Sets bool to pin. Will invert if invert set to true in onstructor
 */
void DigitalIoPin::write(bool b){
	LPC_GPIO->B[port][pin] = (invert) ? !b : b;
}
/**
 * @brief Returns volatile value of pin
 */
volatile bool DigitalIoPin::read(){
	return LPC_GPIO->B[port][pin];
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

