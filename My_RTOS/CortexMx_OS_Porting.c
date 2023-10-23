/*----------------- Includes --------------------------------*/
#include "CortexMx_OS_Porting.h"


/* Global Variables */

volatile uint8_t SystickLed;
/*---------------- Exception Handlers -----------------------*/

/* HardFault Handler */
void HardFault_Handler(void)
{
	while(1);
}

/* MemManage Handler */
void MemManage_Handler(void)
{
	while(1);
}

/* BusFault Handler */
void BusFault_Handler(void)
{
	while(1);
}

/* UsageFault Handler */
void UsageFault_Handler(void)
{
	while(1);
}


/* SVC Handler */
__attribute ((naked)) void SVC_Handler()
{

	/* Compare with SP is used and store it then branch to function */
	__asm volatile("TST LR,#4 \n\t"
			"ITE EQ \n\t"
			"MRSEQ R0,MSP \n\t"
			"MRSNE R0,PSP \n\t"
			"B OS_SVC");

}

/* Systick Handler */
void SysTick_Handler(void){

	SystickLed = 0x1;     /* Set HIGH */

	/* Decide next Task */
	MyRTOS_decideWhatNext();

	/* Trigger PendSV Interrupt */
	OS_TRIGGER_PENDSV();

	SystickLed = 0x0;      /* Set Low */
}


/* Initiliaze Hardware (MCU) */
void HW_Init(void)
{
	/* Set PendSV to be equal or smaller to Systick Priority */
	__NVIC_SetPriority(PendSV_IRQn, 15);

}

void startTicker(void)
{
	/* Interrupt every 1 Ms */
	SysTick_Config(8000);
}
