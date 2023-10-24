/*---------------- Includes ---------------*/
#include "scheduler.h"
#include "FIFO.h"
#include "string.h"

/*--------------- Structs ------------------*/

/* OS Control Struct */
struct {
	Task_Reference* Tasks[NO_OF_MAX_TASKS];    /* Array to HOLD Tasks */
	uint32_t _S_MSP;                           /* Start of Main Stack */
	uint32_t _E_MSP;                           /* End of Main Stack */
	uint32_t Task_Locator_PSP;                 /* Set PSP to which task is excuting */
	uint32_t Number_Of_Active_Tasks;           /* Number of Tasks in active state */
	Task_Reference* Current_Task;              /* Pointer to current Task */
	Task_Reference* Next_Task;                 /* Pointer to current Task */
	enum {
		OS_SUSPENED,
		OS_RUNNING
	}OS_Mode;
}OS_Control;

/* SVC States Enum */
typedef enum {
	ACTIVATE_TASK,
	TERMINATE_TASK,
	WAITING_TASK
}MyRTOS_SVCId;


/*----------------------- Global Variables ---------------- */
FIFO_Buf_t      Ready_Queue;
Task_Reference* Read_Queue_Tasks[100];
Task_Reference  IDLE_TASK;
uint8_t IdleLed;
uint8_t SystickLed;

/*------------------------ Functions -------------------*/

/* Idle Task */
void MyRTOS_IdleTask(void);

/* Creates stack for task */
void MyRTOS_CreateTaskStack(Task_Reference* Task);

/* Creates main stack for OS */
void MyRTOS_SetMainStack(void);

/* SVC Fucntions */
void OS_SVC(uint32_t *Stack_Frame);
void MyRTOS_SVC_Set(MyRTOS_SVCId SVC_ID);

/* Bubble Sort */
void bubbleSort(void);

/* Scheduler */
void MyRTOS_updateSchedulerTable(void);

/* Dispatcher */
void MyRTOS_decideWhatNext(void);

/* Updates waiting time */
void MyRTOS_updateWaitingTime(void);


/* Called by PendSV Handler */
__attribute ((naked)) void OS_PendSV()
{
	/* Save Current task context */
	/* Get current value of PSP */
	OS_GET_PSP(OS_Control.Current_Task->Current_PSP);

	/* Store from R4 to R11 */
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R4" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R5" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R6" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R7" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R8" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R9" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R10" :"=r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)--;
	__asm volatile("MOV %0 ,R11" :"=r" (*(OS_Control.Current_Task->Current_PSP)));


	/* Switch to Next Task */
	if (OS_Control.Next_Task != NULL){
		OS_Control.Current_Task = OS_Control.Next_Task ;
		OS_Control.Next_Task = NULL ;
	}

	/* Restore from memory */
	__asm volatile("MOV R11,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R10,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R9,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R8,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R7,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R6,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R5,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;
	__asm volatile("MOV R4,%0" ::"r" (*(OS_Control.Current_Task->Current_PSP)));
	(OS_Control.Current_Task->Current_PSP)++;

	/* Set PSP */
	OS_SET_PSP(OS_Control.Current_Task->Current_PSP);
	__asm volatile("BX LR");

}

/* Systick Handler */
void OS_Systick(void){

	SystickLed = 0x1;     /* Set HIGH */

	/* Update Waiting Time */
	MyRTOS_updateWaitingTime();

	/* Decide next Task */
	MyRTOS_decideWhatNext();

	/* Trigger PendSV Interrupt */
	OS_TRIGGER_PENDSV();

	SystickLed = 0x0;      /* Set Low */
}


/* Set SVC ID and called in Thread Mode */
void MyRTOS_SVC_Set(MyRTOS_SVCId SVC_ID) {


	switch(SVC_ID)
	{
	case ACTIVATE_TASK:
		__asm("SVC #0x00");
		break;

	case TERMINATE_TASK:
		__asm("SVC #0x01");
		break;

	case WAITING_TASK:
		__asm("SVC #0x02");
		break;
	}
}

/* Function Called by SVC Handler */
/* Handler Mode */
void OS_SVC(uint32_t *Stack_Frame)
{

	uint8_t SVC_Number;
	/* Get SVC Number from memory through R0 */
	SVC_Number = *((uint8_t *)(((uint8_t *)Stack_Frame[6]) - 2));

	switch (SVC_Number) {
	case ACTIVATE_TASK:
		/* Update Scheduler table and ready queue */
		MyRTOS_updateSchedulerTable();

		/* OS is in running state */
		if (OS_Control.OS_Mode == OS_RUNNING) {

			/* If current task is not Idle Task */
			if (strcmp((char*)OS_Control.Current_Task->Task_Name,"IDLE TASK") != 0) {

				/* Decide what next */
				MyRTOS_decideWhatNext();

				/* Trigger PendSV */
				OS_TRIGGER_PENDSV();
			}
		}
		break;
	case TERMINATE_TASK:
		/* Update Scheduler table and ready queue */
		MyRTOS_updateSchedulerTable();

		/* OS is in running state */
		if (OS_Control.OS_Mode == OS_RUNNING) {

			/* If current task is not Idle Task */
			if (strcmp((char*)OS_Control.Current_Task->Task_Name,"IDLE TASK") != 0) {

				/* Decide what next */
				MyRTOS_decideWhatNext();

				/* Trigger PendSV */
				OS_TRIGGER_PENDSV();
			}
		}

		break;
	case WAITING_TASK:

		/* Update Scheduler Table */
		MyRTOS_updateSchedulerTable();
		break;
	}

}


/* Start OS */
void MyRTOS_startOS(void) {

	/* Change OS Mode to Running */
	OS_Control.OS_Mode = OS_RUNNING;

	/* Set Current Task to IDLE */
	OS_Control.Current_Task = &IDLE_TASK;

	/* Activate Idle Task */
	MyRTOS_ActivateTask(&IDLE_TASK);

	/* Start Timer */
	startTicker();

	/* Set PSP to idle task */
	OS_SET_PSP(OS_Control.Current_Task->Current_PSP);

	/* Switch from MSP to PSP */
	OS_SWITCH_SP_TO_PSP();

	/* Switch to unprivilege mode */
	ACCESS_LEVEL_UNPRIVILEGE();

	/* Set Task Entry */
	IDLE_TASK.p_TaskEntry();

}


/* Dispatcher */
void MyRTOS_decideWhatNext(void) {

	/* Queue is empty then put last task */
	if( (Ready_Queue.counter == 0) && (OS_Control.Current_Task->Task_State != TASK_SUSPENDED)) {
		OS_Control.Current_Task->Task_State = TASK_RUNNING;
		FIFO_enqueue(&Ready_Queue,OS_Control.Current_Task);
		OS_Control.Next_Task = OS_Control.Current_Task;
	}
	else {
		FIFO_dequeue(&Ready_Queue,&OS_Control.Next_Task);
		OS_Control.Next_Task->Task_State = TASK_RUNNING;
		//update Ready queue (to keep round robin Algo. happen)
		if ((OS_Control.Current_Task->Task_Priority == OS_Control.Next_Task->Task_Priority )&&(OS_Control.Current_Task->Task_State != TASK_SUSPENDED))
		{
			FIFO_enqueue(&Ready_Queue, OS_Control.Current_Task);
			OS_Control.Current_Task->Task_State = TASK_READY ;
		}
	}
}

/* Scheduler */
void MyRTOS_updateSchedulerTable(void) {

	Task_Reference* Temp = NULL;
	Task_Reference* CurrentTask;
	Task_Reference* NextTask;
	uint8_t i = 0;

	/* Update active tasks in bubble sorting */
	bubbleSort();

	/* Empty the ready queue */
	while(FIFO_dequeue(&Ready_Queue,&Temp) != FIFO_EMPTY);

	/* Fill Items in Ready Queue */
	while(i < OS_Control.Number_Of_Active_Tasks  ) {
		CurrentTask = OS_Control.Tasks[i];
		NextTask = OS_Control.Tasks[i+1];


		if(CurrentTask->Task_State != TASK_SUSPENDED) {

			/* Reached end of array */
			if(NextTask->Task_State == TASK_SUSPENDED ){
				/* Insert Task in queue */
				FIFO_enqueue(&Ready_Queue,CurrentTask);
				/* Change state of task from waiting to ready */
				CurrentTask->Task_State = TASK_READY;
				break;
			}
			if(CurrentTask->Task_Priority < NextTask->Task_Priority) {
				/* Insert Task in queue */
				FIFO_enqueue(&Ready_Queue,CurrentTask);
				/* Change state of task from waiting to ready */
				CurrentTask->Task_State = TASK_READY;
				break;
			}

			else if(CurrentTask->Task_Priority == NextTask->Task_Priority){

				/* Insert Task in queue */
				FIFO_enqueue(&Ready_Queue,CurrentTask);
				/* Change state of task from waiting to ready */
				CurrentTask->Task_State = TASK_READY;
			}
			else if (CurrentTask->Task_Priority > NextTask->Task_Priority)
			{
				//not allowed to happen as we already reordered it by bubble sort
				break ;
			}
		}
		i++;
	}
}


/* Initiliaze RTOS */
void MyRTOS_Init(){


	/* OS to suspend mode */
	OS_Control.OS_Mode = OS_SUSPENED;

	/* Set Main Stack */
	MyRTOS_SetMainStack();

	/* Create OS Ready Queue */
	FIFO_init(&Ready_Queue,Read_Queue_Tasks,100); 
	
	/* Configure IDLE Task */
	strcpy((char*)IDLE_TASK.Task_Name,"IDLE TASK");;    /* Set Idle Task Name */
	IDLE_TASK.Task_Priority = 250;               /* Lowest Priority */
	IDLE_TASK.p_TaskEntry = MyRTOS_IdleTask;     /* Set pointer to function */
	IDLE_TASK.Stack_Size = 300;                  /* Stack Size 100 Bytes */
	MyRTOS_CreateTask(&IDLE_TASK);
}

/* Creates Task */
void MyRTOS_CreateTask(Task_Reference *Task) {


	/* Create Task Stack */
	Task->_S_TaskPSP = OS_Control.Task_Locator_PSP;
	Task->_E_TaskPSP = Task->_S_TaskPSP - Task->Stack_Size;

	/*if(Task->_E_TaskPSP < ((uint32_t)(&_eheap))){
		return ;
	}*/

	/* 8 Bytes Safety margin after Task stack */
	OS_Control.Task_Locator_PSP = Task->_E_TaskPSP - 8;

	/* Create Stack For Task */
	MyRTOS_CreateTaskStack(Task);

	/* Put Task in tasks array */
	OS_Control.Tasks[OS_Control.Number_Of_Active_Tasks] = Task;
	(OS_Control.Number_Of_Active_Tasks)++;


	/* Set Task State to Suspeneded */
	Task->Task_State = TASK_SUSPENDED;


}

/* Activate Task */
void MyRTOS_ActivateTask(Task_Reference *Task) {

	/* Change Task State to waiting from suspneded */
	Task->Task_State = TASK_WAITING;

	/* Call SVC Exception */
	MyRTOS_SVC_Set(ACTIVATE_TASK);

}


/* Idle Task */
void MyRTOS_IdleTask(void){
	while(1) {
		IdleLed ^= 1;
		/*__asm("wfe");*/
	}
}

/* Create Main Stack MSP */
void MyRTOS_SetMainStack(void){

	/* Set start and end of main stack */
	OS_Control._S_MSP = (uint32_t)&_estack ;
	OS_Control._E_MSP = OS_Control._S_MSP - MAIN_STACK_SIZE ;

	/* 8 Bytes Safety margin after main stack */
	OS_Control.Task_Locator_PSP = OS_Control._E_MSP - 8;

}

/* Creates Task Stack PSP */
void MyRTOS_CreateTaskStack(Task_Reference* Task) {

	/* Task Frame */
	/*
	 * Pushed by MC :
	 * XPSR
	 * PC
	 * LR
	 * R12
	 * R3
	 * R2
	 * R1
	 * R0
	 * Pushed by user code :
	 * R4
	 * R5
	 * R6
	 * R7
	 * R8
	 * R9
	 * R11
	 */

	Task->Current_PSP = Task->_S_TaskPSP;                    /* Set current PSP to Start address of stack of task */

	(Task->Current_PSP)--;
	*(Task->Current_PSP) = 0x1000000;                        /* Push Dummy LR to stack , Bit-24 Must be 1 (thumb bit)*/

	(Task->Current_PSP)--;
	*(Task->Current_PSP) = (uint32_t)Task->p_TaskEntry;      /* Set PC to Task Entry */

	(Task->Current_PSP)--;
	*(Task->Current_PSP) = 0xFFFFFFFD ;                      /* LR = 0xFFFFFFFD (EXC_RETURN)Return to thread with PSP */

	/* Set from R0 to R12 with 0x00 */
	for(uint8_t i = 0 ; i < 13 ; i++) {
		(Task->Current_PSP)--;
		*(Task->Current_PSP) = 0x00;
	}
}


/* Terminates Task */
void MyRTOS_TerminateTask(Task_Reference *Task) {

	/* Set Task State */
	Task->Task_State = TASK_SUSPENDED;
	/* Call SVC Exception */
	MyRTOS_SVC_Set(TERMINATE_TASK);

}

/* Put task in suspennded untill time is over */
void MyRTOS_waitTask(uint32_t ticks,Task_Reference *Task) {

	/* Set Timing Wait and state */
	Task->Time_Waiting.Blocking_State = ENABLED;
	Task->Time_Waiting.Ticks_Count = ticks;

	/* Suspend task */
	Task->Task_State = TASK_SUSPENDED;

	/* Terminate Task */
	MyRTOS_SVC_Set(TERMINATE_TASK);
}

/* Update Task Waiting Time */
void MyRTOS_updateWaitingTime(void) {

	/* Loop over active tasks */
	for (int var = 0; var < OS_Control.Number_Of_Active_Tasks; var++) {

		/* Check if Task is in waiting */
		if(OS_Control.Tasks[var]->Task_State == TASK_SUSPENDED) {

			/* Check if Timing waiting is enabled */
			if(OS_Control.Tasks[var]->Time_Waiting.Blocking_State == ENABLED) {

				/* Decrement ticks */
				OS_Control.Tasks[var]->Time_Waiting.Ticks_Count--;

				/* Check if time is ended */
				if(OS_Control.Tasks[var]->Time_Waiting.Ticks_Count == 1) {

					/* Disable time waiting */
					OS_Control.Tasks[var]->Time_Waiting.Blocking_State = DISABLED;

					/* Change state of task to waiting */
					OS_Control.Tasks[var]->Task_State = TASK_WAITING;

					/* Call SVC */
					MyRTOS_SVC_Set(WAITING_TASK);
				}
			}
		}

	}
}


/* Acquire Mutex */
void MyRTOS_AcquierMutex(Mutex_Reference* Mutex,Task_Reference* Task){

	/* Check if mutex is free */
	if(Mutex->Current_Task == NULL) {
		/* Assign mutex to task */
		Mutex->Current_Task = Task;
	}
	else
	{
		if(Mutex->Next_Task == NULL){

			/* Set Mutex to Next Task */
			Mutex->Next_Task = Task;

			/* Change Task state to suppedned */
			Task->Task_State = TASK_SUSPENDED;
			MyRTOS_SVC_Set(TERMINATE_TASK);

		}
		else{

			/* Cannot acquire mutex */
			return ;
		}
	}

}
/* Release Mutex */
void MyRTOS_ReleaseMutex(Mutex_Reference* Mutex){

	if(Mutex->Current_Task != NULL) {

		Mutex->Current_Task = Mutex->Next_Task;
		Mutex->Next_Task = NULL;

		/* Change Task State */
		Mutex->Current_Task->Task_State = TASK_WAITING;
		MyRTOS_SVC_Set(ACTIVATE_TASK);
	}

}


/* Bubble sort tasks  */
void bubbleSort()
{
	unsigned int i, j , n;
	Task_Reference* temp ;
	n = OS_Control.Number_Of_Active_Tasks  ;
	for (i = 0; i < n -1 ; i++)

		// Last i elements are already in place
		for (j = 0; j < n - i - 1; j++)
			if (OS_Control.Tasks[j]->Task_Priority > OS_Control.Tasks[j + 1]->Task_Priority)
			{
				temp = OS_Control.Tasks[j] ;
				OS_Control.Tasks[j] = OS_Control.Tasks[j + 1 ] ;
				OS_Control.Tasks[j + 1] = temp ;
			}

}



