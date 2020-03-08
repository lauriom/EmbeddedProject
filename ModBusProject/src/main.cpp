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
#include "Resources/RingBuffer.h"
#include "Resources/Fan.h"
#include "Resources/PressureSensor.h"
#include "Resources/Watchdog.h"
#include <atomic>
#include <cr_section_macros.h>

#define I2C_MODE (0)
#define I2C_BITRATE (100000)
#define I2C_CLK_DIVIDER (1300)
#define TICKRATE_HZ1 (1000)

const int MAX_FAN_SPEED = 20000;
const int MIN_FAN_SPEED = 2000;
const int FAN_SPEED_STEP = 200; //increment/decrement by this speed at a time when controlling the fan
const int FAN_SPEED_MIN_STEP = 40; //by 1 Hz
const int FAN_SPEED_MAX_STEP = 10000; //max fan freq is 500 Hz. Max step could be 250 Hz (half)
const int PRES_DIFF_MIN_STEP = 1; //pressure difference to use min fan speed step at
const int PRES_DIFF_MAX_STEP = 60; //pressure diff when max fan speed step is used

static volatile std::atomic_int timeout;
static volatile std::atomic_int counter;
static RingBuffer *buffer = new RingBuffer();

#ifdef __cplusplus
extern"C"{
#endif/*
 * @brief Handle interrupt from SysTick timer
 * @returnNothing
 * */
void SysTick_Handler(void){
	if(counter > 0) --counter;

	if(timeout++ == 10000){
		//	buffer->add(ButtonTimeout);
	}
}
void PIN_INT0_IRQHandler(){ // menu Right button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	buffer->add(ButtonRight);
}
void PIN_INT1_IRQHandler(){ // menu Middle button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	buffer->add(ButtonMid);
}
void PIN_INT2_IRQHandler(){ // menu Left button
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
	buffer->add(ButtonLeft);
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

	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(0));

	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(1));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(1));

	Chip_PININT_DisableIntHigh(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(2));
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH(2));


	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	Sleep(10);
}

int clamp(int val, int min, int max); //to clamp values to ranges
int remapRange (int val, int iMin, int iMax, int oMin, int oMax); //input range to output range

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
	Watchdog watchdog(10); //reset time 10 secs

	DigitalIoPin rs = DigitalIoPin(0,8,false,false,false);
	DigitalIoPin en = DigitalIoPin(1,6,false,false,false);
	DigitalIoPin d4 = DigitalIoPin(1,8,false,false,false);
	DigitalIoPin d5 = DigitalIoPin(0,5,false,false,false);
	DigitalIoPin d6 = DigitalIoPin(0,6,false,false,false);
	DigitalIoPin d7 = DigitalIoPin(0,7,false,false,false);

	LiquidCrystal lcd = LiquidCrystal(&rs, &en, &d4, &d5, &d6, &d7);
	MainController menu(&lcd);

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

	Fan fan(&node);
	fan.startFan();
	PressureSensor p;
	int fanFreq = 0;

	//initPinIrq();

	bool autoMode = true;

	for(;;) {

		int paResult = p.readPressureInPa();

		if (autoMode) {

			int targetPressure = 100; //for testing. Replace from UI. from 0 - 120 Pa
			int paResult = p.readPressureInPa();

			printf("Auto mode. Pa result: %d\n", paResult);

			if (paResult != targetPressure) {

				int pressureDiff = abs(targetPressure - paResult);
				int step = remapRange(pressureDiff, PRES_DIFF_MIN_STEP, PRES_DIFF_MAX_STEP, FAN_SPEED_MIN_STEP, FAN_SPEED_MAX_STEP);
				printf("Step is %d\n", step);

				if (paResult < targetPressure) { //adjusting pressure could be improved
					fanFreq += step;
				}

				if (paResult > targetPressure) {
					fanFreq -= step;
				}

				fanFreq = clamp(fanFreq, MIN_FAN_SPEED, MAX_FAN_SPEED); //keep within range
			}

			fan.setFrequency(fanFreq);
		}

		else { //manual mode

			int targetSpeedInPercent = 50; //for testing
			int targetFrequency = remapRange(targetSpeedInPercent, 0, 100, MIN_FAN_SPEED, MAX_FAN_SPEED);

			fan.setFrequency(targetFrequency);
			printf("Manual mode. Pa result: %d\n", paResult);

		}

		/*while((buf = buffer->get()) != 0){// handle interupt events from fifo ringbuf
		menu.eventHandler(buf);
	}*/
		watchdog.feed();
	}

	return 0 ;
}

int clamp(int val, int min, int max) {

	if (val < min) return min;
	if (val > max) return max;
	return val;

}

int remapRange (int val, int iMin, int iMax, int oMin, int oMax) {

	val = clamp(val, iMin, iMax); //restrict value to input range
	float valFraction = float(val - iMin) / float(iMax - iMin); //result between 0 and 1

	return int((oMax - oMin) * valFraction) + oMin; //find where the float would be on the output range
}
