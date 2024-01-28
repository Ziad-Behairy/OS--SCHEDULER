# 🚀 Operating System Process Management Project

## 🌐 Overview

Welcome to the OS Process Management Project! This comprehensive project aims to provide a practical implementation of various process management algorithms in an OS-like environment. By simulating the behavior of these algorithms, users can gain a deep understanding of fundamental concepts such as process scheduling, synchronization, and resource management.

## 🔄 Implemented Algorithms

### 1. First-Come, First-Served (FCFS)

This scheduling algorithm operates on a first-come, first-served basis. Processes are executed in the order they arrive.

### 2. Shortest Job Next (SJN)

SJN, also known as Shortest Job First (SJF), schedules processes based on their burst time. The process with the shortest burst time is selected next.

### 3. Round Robin (RR)

RR is a time-sliced scheduling algorithm, ensuring fair execution by allowing each process to run for a fixed time quantum.

### 4. Priority Scheduling

Processes are scheduled based on their priority. Higher priority processes are given preference.

### 5. Deadlock Detection and Handling

This feature includes the implementation of deadlock detection algorithms to identify and resolve deadlock situations.

### 6. Resource Allocation Graph (RAG)

Visualize and manage resource allocation through a graphical representation of processes and resources.

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

