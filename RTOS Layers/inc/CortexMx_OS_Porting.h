#ifndef CortexMx_OS_Porting_H_
#define CortexMx_OS_Porting_H_

/* ----------------------- Includes ------------------------ */
#include "D:\Embedded Systems\GitHub Projects\RTOS-Scheduler\CMSIS\ARMCM3.h"
#include "D:\Embedded Systems\GitHub Projects\RTOS-Scheduler\CMSIS\core_cm3.h"

/*----------------------- Defines --------------------------*/

/* Set PSP with given address */				
#define OS_SET_PSP(address)           __asm("MSR PSP,%0" :: "r" (address));

/* Return Address of PSP */
#define OS_GET_PSP(address)           __asm("MRS %0,PSP" : "=r" (address));

/* Set access level to unprivilege */ 
/* Set BIT 0 in CONTROL Register */
#define ACCESS_LEVEL_UNPRIVILEGE()    __asm("MRS R0,CONTROL \n\t ORR R0,R0,#0x01 \n\t MSR CONTROL,R0 "); 

/* Set access level to privilege */
/* Clear BIT 0 in CONTROL Register */
#define ACCESS_LEVEL_PRIVILEGE()	    __asm("MRS R0,CONTROL \n\t LSR R0,R0,#0x01 \n\t LSL R0,R0,#0x01 \n\t MSR CONTROL,R0 ");   

/* Switch shadow of SP to PSP */			
#define OS_SWITCH_SP_TO_PSP()         __asm("MRS R0,CONTROL \n\t ORR R0,R0,#0x02 \n\t MSR CONTROL,R0");	 

/* Swicth shadow of SP to MSP */				
#define OS_SWITCH_SP_TO_MSP()         __asm("MRS R0,CONTROL \n\t AND R0,R0,#0x05 \n\t MSR CONTROL,R0");   \

/* Trigger PendSV Interrupt */
#define OS_TRIGGER_PENDSV()           (SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk);

/* Define PendSv Handler */
#define OS_PendSV                     PendSV_Handler

/* Define Systick Handler */      
#define OS_Systick                    SysTick_Handler
/*------------------------ Global Variables --------------------*/
extern uint32_t _estack;                        /* Top of Stack */

/*--------------------------- Functions -------------------------*/

/* Init Required hardware periphales */
void HW_Init(void);

/* Start Timer Function */
void startTicker(void);


#endif /* CortexMx_OS_Porting_H_ */
