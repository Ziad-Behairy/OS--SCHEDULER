# 🚀 Operating System Process Management Project

## 🌐 Overview

Welcome to the OS Process Management Project! This comprehensive project aims to provide a practical implementation of various process management algorithms in an OS-like environment. By simulating the behavior of these algorithms, users can gain a deep understanding of fundamental concepts such as process scheduling, synchronization, and resource management.

## 🔄 Implemented Algorithms

### 1. Round Robin (RR)

The Round Robin scheduling algorithm assigns each process a fixed time slice, often referred to as the time quantum. Processes are executed in a circular manner, with each process getting a turn to run for its allocated time slice. If a process does not complete within its time quantum, it is moved to the end of the queue to await its next turn.

### 2. Shortest Job Next (SJN)

SJN, also known as Shortest Job First (SJF), schedules processes based on their burst time. The process with the shortest burst time is selected next.

### 3. Shortest Remaining Time Next (SRTN)

SRTN is a preemptive scheduling algorithm that selects the process with the smallest remaining execution time for the next execution. The scheduler may interrupt the current process if a new one with a shorter remaining time arrives. SRTN minimizes waiting times and enhances system efficiency by prioritizing processes with the least remaining work.





## 🚀 Getting Started

### Prerequisites

Ensure the following dependencies are installed:

## 🧱 Code Structure

### Header File - `headers.h`

`headers.h` contains clock functions and should be included anywhere the clock functions are used.

To get the current time, call:
```c
getClk();
```
## 🚀 Build and Run Instructions

You can use a Makefile to build and run your project.

### Compile the Project

To compile your project, use the command:

```bash
make
```
## Run the Project
To run your project, use the command:

```bash
make run
```
## Adding Files to the Build Section in the Makefile

If you added a file to your project, add it to the build section in the Makefile. Always start the line with a tab in Makefile; it is its syntax.

### Example Makefile

```makefile
# Example Makefile
CC=gcc
CFLAGS=-I.

main: main.o functions.o
    $(CC) -o main main.o functions.o $(CFLAGS)
```
## Compile and Run All Commands

You can compile and run all the commands in one go with:

```bash
make all
```

