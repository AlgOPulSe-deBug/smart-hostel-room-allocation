# Smart Hostel Room Allocation \& Waiting List Management System

A terminal-based **C application** developed for the **CS-162 Data Structures Project** at IIIT Vadodara – International Campus Diu.

The system automates hostel room allocation using core data structures such as a **Doubly Linked List, Singly Linked List, Binary Max-Heap, and Circular Queue**.

## Project Overview

Manual hostel allocation becomes difficult when many student applications must be processed according to priority, branch compatibility, and room availability.

This project provides a data-structure-driven solution that:

* Registers and validates student applications.
* Calculates a priority score from GPA and home-to-college distance.
* Processes students through a Binary Max-Heap.
* Assigns students to branch-compatible rooms.
* Maintains a circular waiting list when no suitable room is available.
* Automatically promotes waiting students when vacancies become available.
* Supports student search, editing, and deletion.
* Provides room, pending-application, waiting-list, and branch-wise reports.
* Dynamically allocates and frees memory.

## Key Features

### 1\. Student Registration

Student records contain:

* Enrollment ID
* Name
* Branch
* GPA
* Distance from home
* Calculated priority score

Enrollment IDs are validated to contain numbers only and must be unique.

### 2\. Priority-Based Allocation

The priority score is calculated as:

```text
Priority = (GPA × 10) + (Distance ÷ 100)
```

Students with higher priority scores are processed first.

For equal priority scores, the system uses the system ID as a tie-breaker.

### 3\. Branch-Specific Room Allocation

Rooms can be associated with:

* CSE
* ECE
* AI
* MCA
* MTECH
* PHD
* ANY

A room tagged `ANY` can accommodate students from any branch.

When several suitable rooms have free space, the system prefers the room with the highest current occupancy, helping consolidate students into fewer partially occupied rooms.

### 4\. Waiting List

If no suitable room is available, the student enters a **Circular Queue**.

When a room becomes available, a matching waiting-list student can be promoted automatically.

Adding a new room also triggers automatic allocation from the waiting list when branch-compatible students are present.

### 5\. Student Management

The system supports:

* Register
* Edit
* Search
* Delete

Student records can exist in the room allocation structure, pending Max-Heap, or waiting list.

### 6\. Reports

Available reports include:

* Room status
* Waiting list
* Pending applications
* Branch-wise allocation summary

## Data Structures Used

|Data Structure|Purpose|
|-|-|
|Doubly Linked List|Stores hostel rooms|
|Singly Linked List|Stores occupants inside each room|
|Binary Max-Heap|Maintains pending students by priority|
|Circular Queue|Maintains the waiting list|
|Dynamic Memory Allocation|Creates and destroys system structures at runtime|

## System Architecture

```text
                    +----------------------+
                    |   HostelSystem       |
                    +----------+-----------+
                               |
              +----------------+----------------+
              |                |                |
              v                v                v
       +-------------+   +-----------+   +-------------+
       | Room DLL    |   | Max-Heap  |   | Circular    |
       |             |   |           |   | Queue       |
       +------+------+   +-----+-----+   +------+------+
              |                |                |
              v                v                v
       Room + Occupants   Pending Students   Waiting List
```

## Allocation Flow

```text
Register Student
       |
       v
Calculate Priority
       |
       v
Insert into Max-Heap
       |
       v
Extract Highest Priority
       |
       v
Find Matching Room
     /     \\
   Found   Not Found
    |          |
    v          v
 Allocate   Circular
 Student     Queue
    |          |
    +----+-----+
         |
         v
  Vacancy / New Room
         |
         v
 Promote Matching
 Waiting Student
```

## Menu

```text
1.  Register Student
2.  Edit Student Details
3.  Search Student
4.  Delete Student

5.  Allocate Next Applicant
6.  Allocate All Pending
7.  Vacate Room

8.  View Room Status
9.  View Waiting List
10. View Pending Applications
11. Branch-Wise Summary

12. Add New Room
0.  Exit
```

## Project Structure

```text
smart-hostel-room-allocation/
│
├── src/
│   ├── main.c
│   ├── hostel.h
│   ├── allocation.c
│   ├── heap.c
│   ├── queue.c
│   ├── reports.c
│   ├── room.c
│   ├── students.c
│   ├── system.c
│   ├── ui.c
│   └── utils.c
│
├── docs/
│   └── project-presentation.pptx
│
├── legacy/
│   ├── hostel\_v2\_3.c
│   └── prac.c
│
├── Makefile
├── .gitignore
├── LICENSE
└── README.md
```

The `src/` directory contains the modular final implementation. The `legacy/` directory contains earlier/standalone versions retained for reference.

## Requirements

* GCC or another C compiler supporting C11
* Terminal / command prompt

No external libraries are required.

## Build

From the repository root:

### Using Make

```bash
make
```

This creates:

```text
hostel
```

### Using GCC directly

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/\*.c -o hostel
```

## Run

### Linux / macOS

```bash
./hostel
```

### Windows

```powershell
.\\hostel.exe
```

Or, if using MinGW:

```cmd
gcc -std=c11 -Wall -Wextra -pedantic src\\\*.c -o hostel.exe
hostel.exe
```

## Initial System State

At startup, the system creates six default rooms:

```text
CSE
ECE
AI
MCA
MTECH
PHD
```

Each default room starts with a capacity of four occupants.

Additional rooms can be added through the menu.

## Complexity Overview

|Operation|Main Structure|Typical Complexity|
|-|-|-|
|Insert pending student|Max-Heap|O(log n)|
|Extract highest priority|Max-Heap|O(log n)|
|Search rooms|Doubly Linked List|O(n)|
|Append room|Doubly Linked List with tail|O(1)|
|Enqueue waiting student|Circular Queue|O(1)|
|Dequeue waiting student|Circular Queue|O(1)|
|Find matching room|Room traversal|O(r)|
|View rooms|DLL + occupant traversal|O(r + s)|

Here, `n` is the number of pending applications, `r` is the number of rooms, and `s` is the number of stored occupants.

## Memory Management

The program dynamically allocates the main system structures and room/occupant nodes. On exit, the system walks through the room list, frees occupant nodes and room nodes, and then releases the heap, queue, and system structures.

## Example Use Case

Suppose three students apply:

```text
Student A -> GPA 9.0, Distance 100 km
Student B -> GPA 8.5, Distance 700 km
Student C -> GPA 9.2, Distance 50 km
```

Their priority scores are calculated and inserted into the Max-Heap. The highest-priority student is processed first.

If the student's branch has no available room, the student is placed in the waiting list. Once a compatible room becomes available, a matching waiting-list student can be promoted.

## Academic Context

This project demonstrates practical use of:

* Structures in C
* Pointers
* Dynamic memory allocation
* Doubly linked lists
* Singly linked lists
* Binary heaps
* Circular queues
* Searching and traversal
* Priority-based scheduling
* Input validation
* Modular C programming

## Documentation

The `docs/` directory contains the project documentation used to explain the system design, process flow, data structures, and expected behavior.

## License

This project is released under the MIT License. See `LICENSE`.

\---

**Smart Hostel Room Allocation \& Waiting List Management System**  
CS-162 Data Structures Project

