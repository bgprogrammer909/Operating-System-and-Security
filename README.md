# README

## Multi-Threaded Application Using POSIX Threads (C)

### Description

This project demonstrates the basic concepts of Operating Systems using the C programming language and POSIX threads (`pthread`).

The application includes:

* Creation of multiple threads (3 worker threads)
* Thread synchronization using a mutex
* Resource control using a semaphore
* A simple Round Robin scheduling simulation
* Prevention of race conditions
* Deadlock prevention by acquiring and releasing synchronization primitives in a consistent order

---

## Files

* `Task1.c` – Source code
* `README.md` – Project documentation

---

## Requirements

* GCC compiler with POSIX thread support (MinGW-w64 or MSYS2 on Windows)
* Linux users can use the default GCC compiler

---

## Compilation

### Linux

```bash
gcc Task1.c -o Task1 -pthread
```

### Windows (MSYS2 or MinGW-w64)

```bash
gcc Task1.c -o Task1.exe -pthread
```

---

## Execution

### Linux

```bash
./Task1
```

### Windows

```bash
Task1.exe
```

---

## Program Features

### 1. Thread Creation

The program creates three worker threads using `pthread_create()`.

### 2. Synchronization

A mutex protects the shared counter so that only one thread can update it at a time. A semaphore limits the number of threads that may enter the protected section simultaneously.

### 3. Round Robin Scheduling

The program simulates Round Robin CPU scheduling using predefined burst times and a fixed time quantum.

### 4. Race Condition Handling

The shared counter is protected by a mutex to prevent multiple threads from modifying it simultaneously.

### 5. Deadlock Prevention

The program prevents deadlocks by ensuring every thread acquires and releases synchronization objects in the same order.

---

## Expected Output

The program displays:

* Thread creation and execution
* Updates to the shared counter
* Final counter value
* Round Robin scheduling simulation
* Thread completion messages

(The order of thread execution may vary because thread scheduling is controlled by the operating system.)

---

## Author

Name: Suchit Ratna Bajracharya

Course: Operating Systems and Security

