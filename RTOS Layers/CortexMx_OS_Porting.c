/*----------------- Includes --------------------------------*/
#include "CortexMx_OS_Porting.h"


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



/* Initiliaze Hardware (MCU) */
void HW_Init(void)
{
	/* Set PendSV to be equal or smaller to Systick Priority */
	__NVIC_SetPriority(PendSV_IRQn, 15);

}

/* Setup Systick intterupt duration */
void startTicker(void)
{
	/* Interrupt every 1 Ms */
	SysTick_Config(8000);
}
