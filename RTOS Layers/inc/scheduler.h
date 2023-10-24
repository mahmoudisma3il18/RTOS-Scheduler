#ifndef Scheduler_H_
#define Scheduler_H_

/* ---------------------- Includes --------------------- */
#include "CortexMx_OS_Porting.h"


/*----------------------- Defines -------------------------*/
#define NO_OF_MAX_TASKS                         100
#define MAIN_STACK_SIZE                         3072

/*---------------------- Typedefs ----------------------- */

/* Task_Reference */
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

/* Mutex_Reference */
typedef struct{

	uint8_t* p_Payload;
	uint32_t Payload_Size;
	Task_Reference* Current_Task;
	Task_Reference* Next_Task;
	uint8_t Mutex_Name[30];

}Mutex_Reference;

/* FIFO Element */
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

/* Wait Task */
void MyRTOS_waitTask(uint32_t ticks,Task_Reference *Task);

/* Acquire Mutex */
void MyRTOS_AcquierMutex(Mutex_Reference* Mutex,Task_Reference* Task);

/* Release Mutex */
void MyRTOS_ReleaseMutex(Mutex_Reference* Mutex);


#endif /* Scheduler_H_ */
