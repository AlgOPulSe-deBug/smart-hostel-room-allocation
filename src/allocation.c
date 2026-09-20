#include <stdio.h>
#include <stdlib.h>
#include "hostel.h"

// Allocates ONE student (highest priority) from heap to a matching room 
void allocateOne(HostelSystem *sys) {
    if (sys->heap->size == 0) {
        printf("  [i] No pending applications.\n");
        return;
    }
    Student s;
    if (!heapExtractMax(sys->heap, &s)) return;

    printf("  Processing: %-20s  Branch: %-5s  Priority: %d\n",
           s.name, s.branch, s.priorityScore);

    RoomNode *room = findAvailableRoom(sys->rooms, s.branch);
    if (room) {
        addOccupant(room, s);
        printf("  [ALLOCATED] Assigned to Room %d (%s) -- %d/%d occupied.\n",
               room->roomNumber, room->branch,
               room->occupancyCount, room->capacity);
    } else {
        if (enqueue(sys->waitingList, s))
            printf("  [WAITLISTED] No matching room. Added to waiting list.\n");
    }
}

// Menu wrapper: allocates one student, then waits for Enter 
void allocateNextMenu(HostelSystem *sys) {
    HDR("ALLOCATE NEXT APPLICANT");
    allocateOne(sys);
    pressEnter();
}

// Allocates ALL pending students from heap one by one 
void allocateAllMenu(HostelSystem *sys) {
    HDR("ALLOCATE ALL PENDING APPLICANTS");
    if (sys->heap->size == 0) {
        printf("  [i] Priority queue is empty.\n");
    } else {
        while (sys->heap->size > 0) allocateOne(sys);
    }
    pressEnter();
}

// Removes a student from their room and promotes a waiting list student 
void vacateRoom(HostelSystem *sys) {
    HDR("VACATE ROOM / REMOVE STUDENT");

    char target[MAX_ENROLLID_LEN];
    printf("  Enter Enrollment No to remove: ");
    if (scanf(" %19s", target) != 1) { flushIn(); pressEnter(); return; }
    flushIn();

    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *cur  = room->occupants;
        OccupantNode *prev = NULL;
        while (cur) {
            if (strCaseEqual(cur->student.enrollId, target)) {
                // Remove from room 
                if (prev) prev->next = cur->next;
                else       room->occupants = cur->next;
                printf("  [OK] %s (%s) removed from Room %d.\n",
                       cur->student.name, cur->student.enrollId, room->roomNumber);
                free(cur);
                room->occupancyCount--;

                // Promote first matching student from waiting list 
                if (!queueIsEmpty(sys->waitingList)) {
                    int targetIdx = -1;
                    int qpos = sys->waitingList->front;
                    for (int k = 0; k < sys->waitingList->count; k++) {
                        Student *qs = &sys->waitingList->data[qpos];
                        if (branchMatches(room->branch, qs->branch)) {
                            targetIdx = k;
                            break;
                        }
                        qpos = (qpos + 1) % MAX_QUEUE_SIZE;
                    }
                    if (targetIdx >= 0) {
                        Student promoted = sys->waitingList->data[
                            (sys->waitingList->front + targetIdx) % MAX_QUEUE_SIZE];
                        queueRemoveAt(sys->waitingList, targetIdx);
                        addOccupant(room, promoted);
                        printf("  [PROMOTED] %s moved from waiting list to Room %d.\n",
                               promoted.name, room->roomNumber);
                    }
                }
                pressEnter(); return;
            }
            prev = cur; cur = cur->next;
        }
        room = room->next;
    }
    printf("  [!] Enrollment No \"%s\" not found in any room.\n", target);
    pressEnter();
}

/*
 * After a new room is added, scans the waiting list and
 * auto-fills matching students into the new room immediately.
 */
void autoAllocateWaitlist(HostelSystem *sys) {
    if (queueIsEmpty(sys->waitingList)) return;

    RoomNode *room = sys->rooms->tail; /* the room just added */
    int promoted = 0;

    while (room->occupancyCount < room->capacity &&
           !queueIsEmpty(sys->waitingList)) {
        int targetIdx = -1;
        int qpos = sys->waitingList->front;
        for (int k = 0; k < sys->waitingList->count; k++) {
            if (branchMatches(room->branch,
                              sys->waitingList->data[qpos].branch)) {
                targetIdx = k;
                break;
            }
            qpos = (qpos + 1) % MAX_QUEUE_SIZE;
        }
        if (targetIdx < 0) break;

        Student promoted_s = sys->waitingList->data[
            (sys->waitingList->front + targetIdx) % MAX_QUEUE_SIZE];
        queueRemoveAt(sys->waitingList, targetIdx);
        addOccupant(room, promoted_s);
        printf("  [AUTO-ALLOC] %s (%s) moved from waiting list to Room %d.\n",
               promoted_s.name, promoted_s.enrollId, room->roomNumber);
        promoted++;
    }
    if (promoted == 0)
        printf("  [i] No waiting-list students match branch %s.\n", room->branch);
}

// Adds a new room to the DLL and auto-allocates waiting students 
void addNewRoom(HostelSystem *sys) {
    HDR("ADD NEW ROOM");
    char branch[MAX_BRANCH_LEN];
    while (!readBranch("  Branch for this room", branch));
    int cap = readInt("  Capacity (1-8): ", 1, 8);

    if (dllAppendRoom(sys->rooms, sys->nextRoomNumber, cap, branch)) {
        printf("  [OK] Room %d (Branch: %s, Capacity: %d) added.\n",
               sys->nextRoomNumber, branch, cap);
        sys->nextRoomNumber++;
        autoAllocateWaitlist(sys);
    }
    pressEnter();
}