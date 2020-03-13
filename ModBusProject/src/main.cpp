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
#include "Resources/MainController.h"
#include "Resources/Watchdog.h"
#include "Resources/Fan.h"
#include "Resources/PressureSensor.h"
#include "Resources/Menu/LiquidCrystal.h"
#include "Resources/Modbus/LpcUart.h"
#include "ring_buffer.h"

#include <atomic>
#include <cr_section_macros.h>

#define I2C_MODE (0)
#define I2C_BITRATE (100000)
#define I2C_CLK_DIVIDER (1300)
#define TICKRATE_HZ (1000)


const int ButtonRight = 1;
const int ButtonMid = 2;
const int ButtonLeft = 3;
const int debounceTickTime = 150; // button input discarded if pressed within val ms

static volatile std::atomic_uint counter;
static volatile std::atomic_uint sysTick;

RINGBUFF_T *rBuffer = new RINGBUFF_T;

#ifdef __cplusplus
extern"C"{
#endif/*
 * @brief Handle interrupt from SysTick timer
 * @returnNothing
 * */
void SysTick_Handler() {
	if(counter > 0) --counter;
	++sysTick;
}

void PIN_INT0_IRQHandler() { // menu Right button

	static uint32_t prevSysTick = sysTick; // static value initialized on first call

	if((sysTick -  prevSysTick) > debounceTickTime) { //to prevent multiple button presses within a specified time

		RingBuffer_Insert(rBuffer,(void*)&ButtonRight);
		prevSysTick = sysTick;
	}

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0));
}

void PIN_INT1_IRQHandler() { // menu Middle button

	static uint32_t prevSysTick = sysTick; // static value initialized on first call

	if((sysTick -  prevSysTick) > debounceTickTime) {

		RingBuffer_Insert(rBuffer,(void*)&ButtonMid);
		prevSysTick = sysTick;
	}

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(1));
}

void PIN_INT2_IRQHandler() { // menu Left button

	static uint32_t prevSysTick = sysTick; // static value initialized on first call

	if((sysTick -  prevSysTick) > debounceTickTime) {

		RingBuffer_Insert(rBuffer,(void*)&ButtonLeft);
		prevSysTick = sysTick;
	}

	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(2));
}

#ifdef __cplusplus
}
#endif

void Sleep(int ms) {

	counter = ms;

	while(counter > 0) {
		__WFI();
	}
}

uint32_t millis() {

	return counter;
}
/**
 * @Brief Function to enable interrupts on gpio pins D7, D6 AND D4
 * @Note Initialize ringbuffer before calling
 */
static void initPinIrq() {

	Chip_PININT_Init(LPC_GPIO_PIN_INT);

	Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_PININT);
	Chip_SYSCTL_PeriphReset(RESET_PININT);

	// Conf pin D7
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 0);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 0, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN |IOCON_INV_EN));
	Chip_INMUX_PinIntSel(0, 0, 0);

	// Conf pin D6
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 1, 3);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 1, 3, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN |IOCON_INV_EN));
	Chip_INMUX_PinIntSel(1, 1, 3);

	// Conf pin D4
	Chip_GPIO_SetPinDIRInput(LPC_GPIO, 0, 10);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 10, (IOCON_MODE_PULLUP | IOCON_DIGMODE_EN|IOCON_INV_EN));
	Chip_INMUX_PinIntSel(2, 0, 10);

	// Sets interrupt parameters
	Chip_PININT_DisableIntLow (LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));
	Chip_PININT_EnableIntHigh (LPC_GPIO_PIN_INT, PININTCH(0)| PININTCH(1) | PININTCH(2));

	// Enables interupts for pins
	NVIC_ClearPendingIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT0_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);

	NVIC_ClearPendingIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);

	// activate once to init static variables
	PIN_INT0_IRQHandler();
	PIN_INT1_IRQHandler();
	PIN_INT2_IRQHandler();
}

int main(void) {

#if defined (__USE_LPCOPEN)
	// Read clock settings and update SystemCoreClock variable
	SystemCoreClockUpdate();
#if !defined(NO_BOARD_LIB)
	// Set up and initialize all required blocks and
	// functions related to the board hardware
	Board_Init();
#endif
#endif
	if(LPC_WWDT->MOD & 0X4){ // check if watchdog was triggered
		Board_LED_Toggle(0); // sets red led on pernamently
	}

	SysTick_Config(Chip_Clock_GetSysTickClockRate() / TICKRATE_HZ); // 1000 HZ

	Chip_RIT_Init(LPC_RITIMER); //RIT needed for LCD

	//Pins for LCD
	DigitalIoPin rs(0, 8, false, false, false);
	DigitalIoPin en(1, 6, false, false, false);
	DigitalIoPin d4(1, 8, false, false, false);
	DigitalIoPin d5(0, 5, false, false, false);
	DigitalIoPin d6(0, 6, false, false, false);
	DigitalIoPin d7(0, 7, false, false, false);

	LiquidCrystal lcd = LiquidCrystal(&rs, &en, &d4, &d5, &d6, &d7);

	//To enable I2C
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 22, IOCON_DIGMODE_EN |I2C_MODE);
	Chip_IOCON_PinMuxSet(LPC_IOCON, 0, 23, IOCON_DIGMODE_EN |I2C_MODE);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SCL);
	Chip_SWM_EnableFixedPin(SWM_FIXED_I2C0_SDA);
	Chip_I2C_Init(LPC_I2C0);
	Chip_I2C_SetClockDiv(LPC_I2C0, I2C_CLK_DIVIDER);
	Chip_I2CM_SetBusSpeed(LPC_I2C0, I2C_BITRATE);
	Chip_I2CM_Enable(LPC_I2C0);

	NVIC_DisableIRQ(I2C0_IRQn);

	ModbusMaster node(2); // Modbus object that connects to slave id 2
	node.begin(9600);

	Fan fan(&node);
	PressureSensor pressureSensor;
	static int buffer[20];
	RingBuffer_Init(rBuffer, buffer, sizeof(int), 20);
	MainController controller(&lcd, &pressureSensor, &fan, rBuffer); //Initialize controller
	initPinIrq();// starts pin interrupts
	Watchdog watchdog(1); //reset time 1 sec

	int updateCounter = 10;
	while(1) { //

		if(++updateCounter >= 10) {
			// updates on every 10th loop
			controller.updateMenu();
			updateCounter = 0;
		}

		controller.run();
		Sleep(5);
		watchdog.feed();
	}

	delete rBuffer; // loop shouldn't reach this point ,
	return 0;
}

