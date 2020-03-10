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
#include "Resources/Menu/MainController.h"
#include "Resources/Menu/LiquidCrystal.h"
#include "Resources/Modbus/LpcUart.h"
#include "Resources/Menu/RingBuffer.h"
#include "Resources/Fan.h"
#include "Resources/PressureSensor.h"
#include "Resources/Watchdog.h"

#include <atomic>
#include <cr_section_macros.h>

#define I2C_MODE (0)
#define I2C_BITRATE (100000)
#define I2C_CLK_DIVIDER (1300)
#define TICKRATE_HZ1 (1000)

static volatile std::atomic_int timeout;
static volatile std::atomic_int counter;
RingBuffer *buffer; // initialized before pinInterupt is enabled

#ifdef __cplusplus
extern"C"{
#endif/*
 * @brief Handle interrupt from SysTick timer
 * @returnNothing
 * */
void SysTick_Handler(void){
	if(counter > 0) --counter;

	if(++timeout == 10000){
		//	buffer->add(ButtonTimeout);
	}
}
void PIN_INT0_IRQHandler(){ // menu Left button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	buffer->add(ButtonLeft);
}
void PIN_INT1_IRQHandler(){ // menu Middle button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	buffer->add(ButtonMid);
}
void PIN_INT2_IRQHandler(){ // menu Right button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	buffer->add(ButtonRight);
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

uint32_t millis(){
	return counter;
}
/**
 * @Brief Fucntion to enable interupts on gpio pins D7,D6 AND D4
 */
static void initPinIrq(){
	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 0);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 9);
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 10);
	Chip_INMUX_PinIntSel(0, 0, 0);
	Chip_INMUX_PinIntSel(1, 0, 9);
	Chip_INMUX_PinIntSel(2, 0, 10);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN |IOCON_INV_EN ));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 9, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN |IOCON_INV_EN));
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN|IOCON_INV_EN));

	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));

	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	Sleep(10);
}

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
	Chip_RIT_Init(LPC_RITIMER);

	DigitalIoPin rs = DigitalIoPin(0,8,false,false,false);
	DigitalIoPin en = DigitalIoPin(1,6,false,false,false);
	DigitalIoPin d4 = DigitalIoPin(1,8,false,false,false);
	DigitalIoPin d5 = DigitalIoPin(0,5,false,false,false);
	DigitalIoPin d6 = DigitalIoPin(0,6,false,false,false);
	DigitalIoPin d7 = DigitalIoPin(0,7,false,false,false);

	LiquidCrystal lcd = LiquidCrystal(&rs, &en, &d4, &d5, &d6, &d7);

	//	Init_I2C_PinMux();
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN |I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN |I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);

	//	setupI2CMaster();
	Chip_I2C_Init(LPC_I2C0);
	Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);
	Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);
	Chip_I2CM_Enable(LPC_I2C0);

	NVIC_DisableIRQ(I2C0_IRQn);

	LpcPinMap none = {-1, -1}; // unused pin has negative values in it
	LpcPinMap txpin = { 0, 18 }; // transmit pin that goes to debugger's UART->USB converter
	LpcPinMap rxpin = { 0, 13 }; // receive pin that goes to debugger's UART->USB converter
	LpcUartConfig cfg = { LPC_USART0, 115200, UART_CFG_DATALEN_8 | UART_CFG_PARITY_NONE | UART_CFG_STOPLEN_1, false, txpin, rxpin, none, none };
	LpcUart dbgu(cfg);


	ModbusMaster node(2); // Create modbus object that connects to slave id 2
	node.begin(9600);
	buffer = new RingBuffer();
	Fan fan(&node);
	PressureSensor p;
	MainController controller(&lcd,&p,&fan,buffer); // Init controller
	initPinIrq();// starts pin interupts
	Watchdog watchdog(10); //reset time 10 secs

	int updateCounter = 10;
	while(1) {
		if(updateCounter++ == 10){ // updates at start, then 5 times a sec
			controller.updateMenu();
			updateCounter = 0;
		}
		controller.run();
		Sleep(10);
		watchdog.feed();
	}

	delete buffer;
	return 0 ;
}

