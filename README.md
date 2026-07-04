# Multi-Threaded Application Using POSIX Threads (C)

## Description

This project demonstrates the basic concepts of Operating Systems using the C programming language and POSIX threads (`pthread`).

The application includes:

- Creation of multiple threads (3 worker threads)
- Thread synchronization using a mutex
- Resource control using a semaphore
- A simple Round Robin scheduling simulation
- Prevention of race conditions
- Deadlock prevention by acquiring and releasing synchronization primitives in a consistent order

---

## Files

- `Task1.c` – Source code
- `README.md` – Project documentation

---

## Requirements

- GCC compiler with POSIX thread support (MinGW-w64 or MSYS2 on Windows)
- Linux users can use the default GCC compiler

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

## Program Flow Diagram

```text
                   [ Program Start: main() ]
                               │
                               ▼
                     [ Initialize Mutex ]
                               │
                               ▼
                  [ Initialize Semaphore ]
                               │
                               ▼
                     [ Create Threads ]
                               │
                ┌──────────────┴──────────────┐
                ▼                             ▼
         [ Main Thread ]             [ Spawn Worker Thread ]
                │                             │
                │                             ▼
                │                    [ Worker Function ]
                │                             │
                │                             ▼
                │                     [ Semaphore Wait ]
                │                             │
                │                             ▼
                │                       [ Mutex Lock ]
                │                             │
                │                             ▼
                │                  ┌─────────────────────┐
                │                  │  CRITICAL SECTION   │
                │                  │ Update Shared Count │
                │                  └─────────────────────┘
                │                             │
                │                             ▼
                │                      [ Mutex Unlock ]
                │                             │
                │                             ▼
                │                     [ Semaphore Post ]
                │                             │
                │                             ▼
                │                      [ Thread Exit ]
                │                             │
                ▼                             │
┌───────────────────────────────────────────────────────────┐
│             OS Round Robin Scheduling (Simulation)        │
│       Executed after all worker threads have finished     │
└───────────────────────────────────────────────────────────┘
                │
                ▼
         [ Join Threads (pthread_join) ]
                │
                ▼
       [ Print Final Counter Value ]
                │
                ▼
     [ Destroy Mutex & Semaphore ]
                │
                ▼
              [ Program End ]
```

---

## Program Features

### 1. Thread Creation

The program creates three worker threads using `pthread_create()`.

### 2. Synchronization

A mutex protects the shared counter so that only one thread can update it at a time.

A counting semaphore limits access to **two threads**, while the mutex ensures that only one thread updates the shared resource at any given moment.

### 3. Round Robin Scheduling

The program simulates Round Robin CPU scheduling using predefined burst times and a fixed time quantum.

### 4. Race Condition Handling

The shared counter is protected by a mutex to prevent multiple threads from modifying it simultaneously.

### 5. Deadlock Prevention

The program prevents deadlocks by ensuring every thread acquires and releases synchronization objects in the same order.

---

## Expected Output

The program displays:

- Thread creation and execution
- Updates to the shared counter
- Final counter value
- Round Robin scheduling simulation
- Thread completion messages

> **Note:** The order of thread execution may vary because thread scheduling is controlled by the operating system.

---

<img src="https://github.com/bgprogrammer909.png" width="150" height="150" style="border-radius:50%;" alt="Suchit Ratna Bajracharya"/>
</a>

### **Suchit Ratna Bajracharya**

Computer Science Student • Software Developer • AI/ML engineer

<p>
  <a href="https://github.com/bgprogrammer909">
    <img src="https://img.shields.io/badge/GitHub-bgprogrammer909-181717?style=for-the-badge&logo=github" alt="GitHub"/>
  </a>
</p>

🔗 **GitHub Profile:**  
https://github.com/bgprogrammer909

</div>
**Course:** Operating Systems and Security