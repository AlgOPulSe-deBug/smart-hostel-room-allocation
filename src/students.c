#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hostel.h"

/* Registers a new student and adds them to the Max-Heap */
void registerStudent(HostelSystem *sys) {
    HDR("REGISTER NEW STUDENT");

    Student s;
    memset(&s, 0, sizeof(s));
    s.sysId = sys->nextSysId++;

    /* Loop until valid, non-duplicate enrollment ID */
    while (1) {
        printf("  Enrollment No (numbers only): ");
        if (scanf(" %19s", s.enrollId) != 1) {
            flushIn();
            printf("  [!] Invalid input.\n");
            continue;
        }
        flushIn();
        if (!isNumericOnly(s.enrollId)) {
            printf("  [!] Enrollment ID must contain only numbers.\n");
            continue;
        }
        if (enrollIdExists(sys, s.enrollId)) {
            printf("  [!] Enrollment ID \"%s\" already exists.\n", s.enrollId);
            continue;
        }
        break;
    }

    /* Loop until non-empty name */
    do {
        readStr("  Full Name      : ", s.name, MAX_NAME_LEN);
        if (strlen(s.name) == 0) printf("  [!] Name cannot be empty.\n");
    } while (strlen(s.name) == 0);

    while (!readBranch("  Branch", s.branch));
    strToUpper(s.branch);

    /* Loop until valid GPA */
    while (1) {
        printf("  GPA (0.0-10.0) : ");
        if (scanf("%f", &s.gpa) == 1 && s.gpa >= 0.0f && s.gpa <= 10.0f) {
            flushIn();
            break;
        }
        flushIn();
        printf("  [!] Invalid GPA. Must be between 0.0 and 10.0.\n");
    }

    s.distanceKm    = readInt("  Distance (km) : ", 0, 99999);
    s.priorityScore = computePriority(s.gpa, s.distanceKm);

    if (heapInsert(sys->heap, s)) {
        SEP();
        printf("  [OK] Student registered and added to priority queue.\n");
        printf("  Enroll: %-12s  Name: %-20s  Branch: %s\n",
               s.enrollId, s.name, s.branch);
        printf("  GPA: %.2f  |  Distance: %d km  |  Priority Score: %d\n",
               s.gpa, s.distanceKm, s.priorityScore);
        BSEP();
    }

    pressEnter();
}

/* Shows edit sub-menu for a single student. Returns 1 if edited, 0 if cancelled */
int editStudentFields(Student *s) {
    printf("\n  Editing: %s (%s)\n", s->name, s->enrollId);
    SEP();
    printf("  What to edit?\n");
    printf("  1. Name        (currently: %s)\n",   s->name);
    printf("  2. Branch      (currently: %s)\n",   s->branch);
    printf("  3. GPA         (currently: %.2f)\n", s->gpa);
    printf("  4. Distance km (currently: %d)\n",   s->distanceKm);
    printf("  0. Cancel\n");
    SEP();
    int choice = readInt("  Choice: ", 0, 4);

    switch (choice) {
        case 1:
            readStr("  New Name: ", s->name, MAX_NAME_LEN);
            break;
        case 2:
            while (!readBranch("  New Branch", s->branch));
            strToUpper(s->branch);
            break;
        case 3: {
            float tmp;
            printf("  New GPA: ");
            if (scanf("%f", &tmp) != 1 || tmp < 0.0f || tmp > 10.0f) {
                flushIn();
                printf("  [!] Invalid GPA. Must be 0.0 - 10.0.\n");
                return 0;
            }
            flushIn();
            s->gpa = tmp;
            break;
        }
        case 4:
            s->distanceKm = readInt("  New Distance (km): ", 0, 99999);
            break;
        case 0:
            return 0;
    }
    s->priorityScore = computePriority(s->gpa, s->distanceKm);
    return 1;
}

/* Finds a student by enrollment ID across DLL/heap/queue and edits them */
void editStudent(HostelSystem *sys) {
    HDR("EDIT STUDENT DETAILS");

    char target[MAX_ENROLLID_LEN];
    printf("  Enter Enrollment No to edit: ");
    if (scanf(" %19s", target) != 1) { flushIn(); return; }
    flushIn();

    /* Search in rooms */
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseEqual(occ->student.enrollId, target)) {
                if (editStudentFields(&occ->student))
                    printf("  [OK] Student updated (Room %d).\n", room->roomNumber);
                pressEnter(); return;
            }
            occ = occ->next;
        }
        room = room->next;
    }

    /* Search in heap — if edited, reorder heap */
    for (int i = 0; i < sys->heap->size; i++) {
        if (strCaseEqual(sys->heap->data[i].enrollId, target)) {
            if (editStudentFields(&sys->heap->data[i])) {
                shiftUp(sys->heap, i);
                shiftDown(sys->heap, i);
                printf("  [OK] Student updated (pending queue).\n");
            }
            pressEnter(); return;
        }
    }

    /* Search in waiting list */
    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, target)) {
            if (editStudentFields(&sys->waitingList->data[pos]))
                printf("  [OK] Student updated (waiting list).\n");
            pressEnter(); return;
        }
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }

    printf("  [!] Enrollment No \"%s\" not found.\n", target);
    pressEnter();
}

// Permanently deletes a student from any location in the system 
void deleteStudent(HostelSystem *sys) {
    HDR("DELETE STUDENT FROM SYSTEM");

    char target[MAX_ENROLLID_LEN];
    printf("  Enter Enrollment No to delete: ");
    if (scanf(" %19s", target) != 1) { flushIn(); pressEnter(); return; }
    flushIn();

    /* Search in rooms */
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *cur  = room->occupants;
        OccupantNode *prev = NULL;
        while (cur) {
            if (strCaseEqual(cur->student.enrollId, target)) {
                printf("\n  Found: %s (%s) in Room %d\n",
                       cur->student.name, cur->student.enrollId, room->roomNumber);
                int confirm = readInt("  Are you sure? (1=Yes, 0=No): ", 0, 1);
                if (confirm == 1) {
                    if (prev) prev->next = cur->next;
                    else       room->occupants = cur->next;
                    printf("  [OK] %s deleted from Room %d.\n",
                           cur->student.name, room->roomNumber);
                    free(cur);
                    room->occupancyCount--;
                } else {
                    printf("  [!] Deletion cancelled.\n");
                }
                pressEnter(); return;
            }
            prev = cur; cur = cur->next;
        }
        room = room->next;
    }

    /* Search in heap */
    for (int i = 0; i < sys->heap->size; i++) {
        if (strCaseEqual(sys->heap->data[i].enrollId, target)) {
            printf("\n  Found: %s (%s) in Pending Applications\n",
                   sys->heap->data[i].name, sys->heap->data[i].enrollId);
            int confirm = readInt("  Are you sure? (1=Yes, 0=No): ", 0, 1);
            if (confirm == 1) {
                printf("  [OK] %s deleted from pending queue.\n",
                       sys->heap->data[i].name);
                heapRemoveAt(sys->heap, i);
            } else {
                printf("  [!] Deletion cancelled.\n");
            }
            pressEnter(); return;
        }
    }

    /* Search in waiting list */
    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, target)) {
            printf("\n  Found: %s (%s) in Waiting List\n",
                   sys->waitingList->data[pos].name,
                   sys->waitingList->data[pos].enrollId);
            int confirm = readInt("  Are you sure? (1=Yes, 0=No): ", 0, 1);
            if (confirm == 1) {
                printf("  [OK] %s deleted from waiting list.\n",
                       sys->waitingList->data[pos].name);
                queueRemoveAt(sys->waitingList, i);
            } else {
                printf("  [!] Deletion cancelled.\n");
            }
            pressEnter(); return;
        }
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }

    printf("  [!] Enrollment No \"%s\" not found in system.\n", target);
    pressEnter();
}

/* Searches for a student by name or enrollment ID across entire system */
void searchStudent(const HostelSystem *sys) {
    HDR("SEARCH STUDENT");
    char query[MAX_NAME_LEN];
    readStr("  Name or Enrollment No (partial ok): ", query, MAX_NAME_LEN);

    if (strlen(query) == 0) {
        printf("  [!] Search term cannot be empty.\n");
        pressEnter(); return;
    }

    int found = 0;

    /* Search rooms */
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseStr(occ->student.name, query) ||
                strCaseStr(occ->student.enrollId, query)) {
                printf("  [Room %3d] %-12s  %-18s  Branch:%-5s  GPA:%.1f  Pri:%d\n",
                       room->roomNumber, occ->student.enrollId,
                       occ->student.name, occ->student.branch,
                       occ->student.gpa, occ->student.priorityScore);
                found++;
            }
            occ = occ->next;
        }
        room = room->next;
    }

    /* Search waiting list */
    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        Student *s = &sys->waitingList->data[pos];
        if (strCaseStr(s->name, query) || strCaseStr(s->enrollId, query)) {
            printf("  [Waitlist] %-12s  %-18s  Branch:%-5s  GPA:%.1f  Pri:%d\n",
                   s->enrollId, s->name, s->branch, s->gpa, s->priorityScore);
            found++;
        }
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }

    /* Search heap */
    for (int i = 0; i < sys->heap->size; i++) {
        Student *s = &sys->heap->data[i];
        if (strCaseStr(s->name, query) || strCaseStr(s->enrollId, query)) {
            printf("  [Pending]  %-12s  %-18s  Branch:%-5s  GPA:%.1f  Pri:%d\n",
                   s->enrollId, s->name, s->branch, s->gpa, s->priorityScore);
            found++;
        }
    }

    SEP();
    printf("  %d result(s) found.\n", found);
    pressEnter();
}