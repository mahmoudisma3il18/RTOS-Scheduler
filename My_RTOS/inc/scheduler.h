#ifndef Scheduler_H_
#define Scheduler_H_

/* ---------------------- Includes --------------------- */
#include "CortexMx_OS_Porting.h"



/*----------------------- Defines -------------------------*/
#define NO_OF_MAX_TASKS                         100
#define MAIN_STACK_SIZE                         3072

/*---------------------- Typedefs ----------------------- */
typedef enum {
	NO_ERROR,
	FIFO_ERROR,
	TASK_SIZE_ERROR
}MyRTOS_ErrorType;




typedef struct {
	uint8_t Task_Name[30];       /* Holds Task Name */
	uint32_t Stack_Size;         /* Task Stack Size */
	uint8_t  Task_Priority;      /* Task Priority */
	void (*p_TaskEntry)(void);   /* Pointer to task entry */
	uint32_t _S_TaskPSP;         /* Start address of Task */
	uint32_t _E_TaskPSP;         /* End address of Task */
	uint32_t *Current_PSP;       /* Pointer to current instruction in task */
	enum {
		TASK_SUSPENDED,
		TASK_RUNNING,
		TASK_WAITING,
		TASK_READY
	}Task_State;
	struct {
		enum {
			ENABLED,
			DISABLED
		}Blocking_State;
		uint32_t Ticks_Count;
	}Time_Waiting;
}Task_Reference;


#define element_type                            Task_Reference*

/* -------------------- Prototypes ---------------------- */

/* Init Task */
void MyRTOS_Init(void);
/* Creates Task */
void MyRTOS_CreateTask(Task_Reference *Task);
/* Activates Task */
void MyRTOS_ActivateTask(Task_Reference *Task);
/* Terminates Task */
void MyRTOS_TerminateTask(Task_Reference *Task);
/* Starts OS */
void MyRTOS_startOS(void);


#endif /* Scheduler_H_ */
