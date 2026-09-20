# Smart Hostel Room Allocation & Waiting List Management System

## 1. Project Overview

The **Smart Hostel Room Allocation & Waiting List Management System** is a terminal-based C application designed to manage hostel room allocation, student applications, room vacancies, and waiting-list operations.

The project demonstrates the practical use of fundamental data structures and modular programming in C.

The system manages:

- Student registration and records
- Hostel rooms and room occupancy
- Priority-based pending applications
- Waiting-list management
- Room allocation and vacation
- Automatic promotion of waiting students
- Hostel and branch-wise reports

---

## 2. Objectives

The main objectives of the project are:

1. To automate the hostel room allocation process.
2. To prioritize students using a calculated priority score.
3. To maintain room and occupant information dynamically.
4. To implement a waiting-list mechanism when rooms are unavailable.
5. To demonstrate the use of linked lists, heaps, and circular queues.
6. To provide a modular C implementation that is easy to maintain and extend.
7. To practice file organization, function modularity, and dynamic memory management in C.

---

## 3. Technologies Used

| Component | Technology |
|---|---|
| Programming Language | C |
| Standard | C11 |
| Compiler | GCC |
| Interface | Terminal / Command Line |
| Build System | Makefile |
| Version Control | Git / GitHub |

The project is compiled with warnings enabled using:

```bash
gcc -std=c11 -Wall -Wextra -pedantic
```

---

## 4. Data Structures Used

### 4.1 Doubly Linked List

A **Doubly Linked List** is used to maintain hostel rooms.

Each room can be traversed in both forward and backward directions.

It supports operations such as:

- Adding rooms
- Searching rooms
- Displaying rooms
- Managing room availability

---

### 4.2 Singly Linked List

A **Singly Linked List** is used to maintain the occupants of a room.

It supports dynamic insertion and removal of students without requiring a fixed-size array.

---

### 4.3 Binary Max-Heap

A **Binary Max-Heap** is used for pending hostel applications.

Students are ordered according to their priority score so that the highest-priority application can be processed first.

The priority formula used by the system is:

```text
Priority = (GPA × 10) + (Distance ÷ 100)
```

A higher priority score means the application is processed earlier by the allocation system.

---

### 4.4 Circular Queue

A **Circular Queue** is used for the hostel waiting list.

When rooms are unavailable, students can be placed into the waiting queue. When a room becomes available, the system can promote a waiting student into the allocation process.

---

## 5. System Architecture

The project follows a modular structure instead of keeping the entire application in a single C file.

```text
                    +----------------------+
                    |       main.c         |
                    |   Program / Menu     |
                    +----------+-----------+
                               |
                               v
                    +----------------------+
                    |      system.c        |
                    | Initialization /     |
                    | System-level logic   |
                    +----------+-----------+
                               |
          +--------------------+--------------------+
          |                    |                    |
          v                    v                    v
 +----------------+   +----------------+   +----------------+
 |   students.c   |   |   allocation.c |   |    room.c      |
 | Student data   |   | Allocation     |   | Room handling  |
 +----------------+   +----------------+   +----------------+
          |                    |                    |
          +--------------------+--------------------+
                               |
                    +----------+-----------+
                    |                      |
                    v                      v
             +-------------+       +-------------+
             |   heap.c    |       |   queue.c   |
             | Max Heap    |       | Circular Q  |
             +-------------+       +-------------+
                               |
                               v
                    +----------------------+
                    |      reports.c       |
                    | Reports / Summaries   |
                    +----------------------+
```

---

## 6. Source Code Organization

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
├── Makefile
├── README.md
├── LICENSE
└── docs/
    └── PROJECT_DOCUMENTATION.md
```

---

## 7. Module Description

### `main.c`

Contains the program entry point and starts the hostel management application.

### `hostel.h`

Contains common structures, constants, declarations, and interfaces shared by the modules.

### `allocation.c`

Handles hostel allocation operations, including allocation from the pending applications and promotion of students when rooms become available.

### `heap.c`

Implements the Binary Max-Heap used to maintain pending applications according to priority.

### `queue.c`

Implements the Circular Queue used for waiting-list management.

### `room.c`

Handles hostel room operations such as room creation, room searching, room availability, and occupant management.

### `students.c`

Handles student-related operations such as registration, editing, searching, and deletion.

### `system.c`

Handles system initialization and system-level setup.

### `reports.c`

Generates hostel and branch-related summaries and reports.

### `ui.c`

Contains user-interface and menu-related functions for the terminal application.

### `utils.c`

Contains utility/helper functions shared by different modules.

---

## 8. Priority-Based Allocation

When a student applies for hostel accommodation, a priority score is calculated.

The formula is:

```text
Priority = (GPA × 10) + (Distance ÷ 100)
```

Where:

- `GPA` represents the student's academic performance.
- `Distance` represents the student's distance from the hostel/home location as used by the system.
- The calculated score is used by the Max-Heap.

Example:

```text
GPA = 8.5
Distance = 500 km

Priority = (8.5 × 10) + (500 ÷ 100)
         = 85 + 5
         = 90
```

The Binary Max-Heap allows the application with the highest priority to be retrieved efficiently.

---

## 9. Main System Operations

### Student Operations

The system supports:

- Register student
- Edit student details
- Search student
- Delete student
- View student/application information

### Room Operations

The system supports:

- Add room
- Search room
- Check room availability
- View room occupancy
- Vacate room
- Add occupants

### Allocation Operations

The system supports:

- Allocate the next eligible student
- Allocate all possible pending applications
- Allocate students when rooms become available
- Promote students from the waiting list

### Waiting List Operations

The system supports:

- Add student to waiting list
- View waiting list
- Remove/promote waiting students when capacity becomes available

### Reports

The system provides:

- Room/occupancy information
- Waiting-list information
- Pending application information
- Branch-wise summaries

---

## 10. Allocation Flow

The general allocation process is:

```text
Student Application
        |
        v
Calculate Priority
        |
        v
Insert into Max-Heap
        |
        v
Check Room Availability
        |
        +------ Room Available ------+
        |                             |
        |                             v
        |                    Allocate Student
        |                             |
        |                             v
        |                     Update Room
        |
        +------ No Room Available ---+
                                      |
                                      v
                              Waiting Queue
                                      |
                                      v
                              Room Becomes Free
                                      |
                                      v
                              Promote Student
```

---

## 11. Room Vacation and Promotion

When an occupant vacates a room:

1. The student's occupancy record is updated.
2. The room becomes available.
3. The system checks the waiting list/pending allocation state.
4. An eligible waiting student can be promoted.
5. The room occupancy is updated.

This avoids leaving available hostel capacity unused while applications are waiting.

---

## 12. Adding a New Room

The system can add a new hostel room dynamically.

After a room is added:

1. The new room becomes available.
2. The system checks whether students are waiting.
3. Waiting students can be processed automatically.
4. The room can be assigned according to the system's allocation rules.

---

## 13. Compilation

Using GCC:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/*.c -o hostel
```

On Windows using MinGW:

```bash
gcc -std=c11 -Wall -Wextra -pedantic src/*.c -o hostel.exe
```

The project also contains a `Makefile` for build automation.

---

## 14. Running the Program

After compilation:

### Windows

```bash
hostel.exe
```

### Linux / macOS

```bash
./hostel
```

The application runs through a terminal-based menu.

---

## 15. Design Principles

The project follows these principles:

- Modular programming
- Separation of responsibilities
- Dynamic data structures
- Reusable functions
- Clear interfaces through header files
- Standard C programming practices
- Compiler warning checks
- Avoidance of unnecessary global state

---

## 16. Testing

The source code was compiled using:

```bash
gcc -std=c11 -Wall -Wextra -pedantic
```

The compilation was checked for compiler warnings and errors.

The project structure was also verified to ensure that the application uses the modular implementation under `src/`.

---

## 17. Version Control

The project is maintained using Git.

The repository contains the source code, build configuration, documentation, and license.

The project does not require the previous monolithic/legacy implementation for the current modular build.

---

## 18. Future Improvements

Possible future improvements include:

- Persistent database/file storage for student records
- Authentication and role-based access
- Graphical user interface
- Web-based hostel management portal
- Advanced reporting and analytics
- Automated room optimization
- More detailed student preference handling
- Exporting reports to CSV/PDF
- Improved input validation and error handling

---

## 19. Conclusion

The Smart Hostel Room Allocation & Waiting List Management System demonstrates how fundamental data structures can be combined to solve a practical resource-allocation problem.

The project integrates:

```text
Doubly Linked List
        +
Singly Linked List
        +
Binary Max-Heap
        +
Circular Queue
        =
Hostel Allocation System
```

The modular C implementation separates different responsibilities into individual source files, making the project easier to understand, compile, test, maintain, and extend.
