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
git clone https://github.com/your-username/MyRTOS.git
