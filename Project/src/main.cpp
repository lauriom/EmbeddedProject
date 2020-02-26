/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
 */

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif
#include<atomic>
static volatile std::atomic_int counter;
#ifdef __cplusplus
extern"C"{
#endif/*
 * @brief Handle interrupt from SysTick timer
 * @returnNothing
 * */
void SysTick_Handler(void){
	if(counter > 0) counter--;
}
#ifdef __cplusplus
}
#endif
void Sleep(int ms){
	counter = ms;
	while(counter > 0) {
		__WFI();
	}
}

#include <cr_section_macros.h>

// TODO: insert other include files here
#include "Resources/DigitalIoPin.h"
#include "Resources/Modbus/ModbusMaster.h"

// TODO: insert other definitions and declarations here
#define TICKRATE_HZ1 1000
int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
	// Set the LED to the state of "On"
#endif
#endif
	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ1);

	ModbusMaster node(2);
	node.begin(9600);
	DigitalIoPin red(0,25,false);

	static uint32_t i;
	uint8_t j,result;
	uint16_t data[6];
 	while(1){

 		for(int j = 0; j < 2;j++){
 			node.setTransmitBuffer(j, ++i & 0xFFFF);
 		}
 		result = node.writeMultipleRegisters(0,j);
 		result = node.readHoldingRegisters(2, 6);
	}
	return 0 ;
}
