# MyRTOS - Real-Time Operating System for Cortex-M3

MyRTOS is a real-time operating system designed for the ARM Cortex-M3 microcontroller platform. It provides a priority preemptive round-robin scheduler and a set of APIs to help you manage tasks and synchronization in your embedded systems projects.

## Features

- **Preemptive Round-Robin Scheduling:** MyRTOS uses a preemptive round-robin scheduling algorithm to ensure fair execution of tasks with different priorities.

- **Task Management:** Easily create, activate, and terminate tasks in your embedded applications.

- **Timing Control:** Implement time-based waiting for tasks using the `MyRTOS_waitTask` function.

- **Mutex Support:** Efficiently manage shared resources using the `MyRTOS_AcquireMutex` and `MyRTOS_ReleaseMutex` functions.

## Getting Started

To start using MyRTOS in your Cortex-M3 projects, follow these simple steps:

### Prerequisites

- ARM Cortex-M3 development environment (e.g., ARM Keil, STM32CubeIDE, etc.)

### Installation

1. Clone this repository or download the MyRTOS source code.
```bash
git clone https://github.com/mahmoudisma3il18/RTOS-Scheduler.git
```

2. Include the MyRTOS source files in your project.
3. Configure the MyRTOS according to your system requirements, such as stack sizes, maximum tasks, etc.
4. Initialize MyRTOS using the `MyRTOS_Init` function in your application's setup code.
5. Create tasks using the `MyRTOS_CreateTask` function and implement your task functions.
6. Start the operating system using `MyRTOS_startOS` to begin task scheduling.

### Usage

To use MyRTOS in your application, follow these steps:

1. **Initializing MyRTOS**:
```c
MyRTOS_Init();
```
2. **Creating a Task**:
 ```c
Task_Reference myTask;
MyRTOS_CreateTask(&myTask);
```
3. **Activating a Task**:
```c
MyRTOS_ActivateTask(&myTask);
```
4. **Terminating a Task**: 
```c
MyRTOS_TerminateTask(&myTask);
```
5. **Starting the Operating System**:
```c
MyRTOS_startOS();
```
6. **Waiting in a Task**:
```c
MyRTOS_waitTask(ticks, &myTask);
```
7. **Acquiring and Releasing Mutex**:
```c
Mutex_Reference myMutex;
MyRTOS_AcquierMutex(&myMutex, &myTask);
MyRTOS_ReleaseMutex(&myMutex);
```

### Images
![Round Robin Algo](https://github.com/mahmoudisma3il18/RTOS-Scheduler/assets/103974340/a1f9ebad-0e33-4b76-a86f-89899d98ab5a)
![Priority Inversion](https://github.com/mahmoudisma3il18/RTOS-Scheduler/assets/103974340/8d0a4d09-094a-45d8-83a5-71625e497339)
![Priority Preemptive Round Roubin](https://github.com/mahmoudisma3il18/RTOS-Scheduler/assets/103974340/05c42948-685f-44d9-8cd0-e1ea8c1778e0)




