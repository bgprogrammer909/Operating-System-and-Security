# Operating Systems and Security

A collection of **Operating Systems** laboratory assignments developed in **C**, demonstrating fundamental OS concepts including **multithreading, memory management, CPU scheduling, and network programming** using POSIX APIs.

The repository is designed for educational purposes and showcases practical implementations of synchronization mechanisms, scheduling algorithms, memory management techniques, and TCP client-server communication.

---

## Project Structure

```text
Operating-System-and-Security/
│
├── Task1.c              # POSIX Threads & Synchronization
├── Task2.c              # Memory Management
├── Task3.c              # CPU Scheduling
├── Task4Server.c        # Multithreaded TCP Server
├── Task4Client.c        # Interactive/Demo TCP Client
├── Task4Clients.c       # Concurrent Client Test
└── README.md
```

---

# Tasks Overview

| Task       | Description                                                                                                                                                               |
| ---------- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **Task 1** | Multi-threaded application using POSIX Threads (`pthread`) with mutexes, semaphores, race condition handling, deadlock prevention, and Round Robin scheduling simulation. |
| **Task 2** | Memory Management simulation demonstrating memory allocation and management concepts.                                                                                     |
| **Task 3** | CPU Scheduling simulation implementing process scheduling algorithms and performance calculations.                                                                        |
| **Task 4** | Multi-threaded TCP Client-Server application implementing authentication, command processing, and concurrent client handling.                                             |

---

# Features

### Task 1

* POSIX Threads (`pthread`)
* Mutex Synchronization
* Counting Semaphore
* Shared Resource Protection
* Race Condition Prevention
* Deadlock Prevention
* Round Robin Scheduling Simulation

### Task 2

* Memory Management Simulation
* Dynamic Memory Allocation
* Memory Block Management
* Allocation & Deallocation
* Memory Statistics

### Task 3

* CPU Scheduling Algorithms
* Waiting Time Calculation
* Turnaround Time Calculation
* Scheduling Simulation
* Performance Evaluation

### Task 4

* TCP Socket Programming
* POSIX Threads
* Multi-threaded Server
* Client Authentication
* Interactive Command Processing
* Concurrent Client Support
* Input Validation
* Network Communication

---

# Program Architecture

```text
                     Operating Systems
                             │
      ┌──────────────────────┼──────────────────────┐
      ▼                      ▼                      ▼
  Multithreading      Memory Management      CPU Scheduling
      │                      │                      │
      └──────────────────────┴──────────────────────┘
                             │
                             ▼
                  TCP Network Programming
```

---

# Task Workflow

```text
                 Start
                   │
                   ▼
          Select Required Task
                   │
      ┌────────────┼─────────────┐
      ▼            ▼             ▼
   Task 1       Task 2        Task 3
      │            │             │
      └────────────┴─────────────┘
                   │
                   ▼
               Task 4
                   │
                   ▼
            Display Results
```

---

# Requirements

* GCC Compiler
* POSIX Threads (`pthread`)
* TCP Socket Support
* Linux / macOS

### Windows

* MSYS2
* MinGW-w64

---

# Compilation

## Task 1

### Linux

```bash
gcc Task1.c -o Task1 -pthread
```

### Windows

```bash
gcc Task1.c -o Task1.exe -pthread
```

---

## Task 2

### Linux

```bash
gcc Task2.c -o Task2
```

### Windows

```bash
gcc Task2.c -o Task2.exe
```

---

## Task 3

### Linux

```bash
gcc Task3.c -o Task3
```

### Windows

```bash
gcc Task3.c -o Task3.exe
```

---

## Task 4

### Compile Server

```bash
gcc Task4Server.c -o Server -pthread
```

### Compile Client

```bash
gcc Task4Client.c -o Client
```

### Compile Concurrent Client Test

```bash
gcc Task4Clients.c -o MultiClient -pthread
```

---

# Execution

## Task 1

### Linux

```bash
./Task1
```

### Windows

```bash
Task1.exe
```

---

## Task 2

### Linux

```bash
./Task2
```

### Windows

```bash
Task2.exe
```

---

## Task 3

### Linux

```bash
./Task3
```

### Windows

```bash
Task3.exe
```

---

## Task 4

### Start Server

```bash
./Server
```

### Start Client

```bash
./Client
```

### Demo Mode

```bash
./Client --demo
```

### Concurrent Client Test

```bash
./MultiClient
```

---

# Network Communication

```text
                  +---------------------+
                  |  Multithreaded      |
                  |      Server         |
                  +----------+----------+
                             │
        ┌────────────────────┼────────────────────┐
        ▼                    ▼                    ▼
   Client 1             Client 2             Client 3
        │                    │                    │
      AUTH                 AUTH                AUTH
      ECHO                 TIME                 ADD
      QUIT                 QUIT                QUIT
```

---

# Task Summary

## Task 1

* Creates multiple worker threads
* Synchronizes shared resources
* Uses mutexes and semaphores
* Simulates Round Robin Scheduling

---

## Task 2

* Simulates memory allocation
* Demonstrates memory management concepts
* Allocates and releases memory blocks

---

## Task 3

* Simulates CPU scheduling
* Calculates waiting and turnaround times
* Displays scheduling performance

---

## Task 4

* Starts a TCP server
* Accepts multiple clients simultaneously
* Authenticates users
* Processes client commands
* Supports concurrent communication

---

# Technologies Used

* C Programming
* POSIX Threads (`pthread`)
* GCC Compiler
* TCP/IP Socket Programming
* Operating System Concepts

---

# Expected Output

Each task demonstrates different Operating System concepts:

* Thread creation and synchronization
* Memory allocation and management
* CPU scheduling simulation
* Client-server communication
* Concurrent client handling
* Authentication and command processing

> **Note:** Thread execution order and concurrent client behaviour may vary depending on the operating system scheduler.

---

<img src="https://github.com/bgprogrammer909.png" width="150" height="150" style="border-radius:50%;" alt="Suchit Ratna Bajracharya"/>

## Suchit Ratna Bajracharya

**Computer Science Student • Software Developer • AI/ML Engineer**

<p>
  <a href="https://github.com/bgprogrammer909">
    <img src="https://img.shields.io/badge/GitHub-bgprogrammer909-181717?style=for-the-badge&logo=github" alt="GitHub"/>
  </a>
</p>

**GitHub Profile**
https://github.com/bgprogrammer909

---

## Course

**Operating Systems and Security**
