#include <stdio.h>
#include "hostel.h"

/* Returns VACANT / PARTIAL / FULL label for a room */
const char *statusTag(const RoomNode *r) {
    if (r->occupancyCount == 0)           return "VACANT  ";
    if (r->occupancyCount == r->capacity) return "FULL    ";
    return "PARTIAL ";
}

/* Displays all rooms in the DLL with their occupants */
void viewRooms(const HostelSystem *sys) {
    HDR("ROOM STATUS (Doubly Linked List)");

    RoomNode *cur = sys->rooms->head;
    if (!cur) { printf("  No rooms registered.\n"); pressEnter(); return; }

    while (cur) {
        printf("  | Room %-3d  Branch: %-5s  [%d/%d]  Status: %s  |\n",
               cur->roomNumber, cur->branch,
               cur->occupancyCount, cur->capacity, statusTag(cur));
        SEP();

        OccupantNode *occ = cur->occupants;
        if (!occ)
            printf("  |   (vacant)                                              |\n");
        while (occ) {
            printf("  |  Enroll:%-12s  %-16s  GPA:%.1f  Pri:%-3d  |\n",
                   occ->student.enrollId, occ->student.name,
                   occ->student.gpa, occ->student.priorityScore);
            occ = occ->next;
        }

        if (cur->next) BSEP();
        cur = cur->next;
    }
    BSEP();
    printf("  Total Rooms: %d\n", sys->rooms->size);
    pressEnter();
}

/* Displays all students in the circular queue (waiting list) */
void viewWaitingList(const HostelSystem *sys) {
    HDR("WAITING LIST (Circular Queue)");

    const CircularQueue *q = sys->waitingList;
    if (queueIsEmpty(q)) {
        printf("  Waiting list is empty.\n"); BSEP(); pressEnter(); return;
    }

    int pos = q->front;
    for (int i = 0; i < q->count; i++) {
        printf("   #%-3d  Enroll:%-12s  %-16s  Branch:%-5s  Pri:%-3d \n",
               i + 1,
               q->data[pos].enrollId, q->data[pos].name,
               q->data[pos].branch,   q->data[pos].priorityScore);
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }
    BSEP();
    printf("  Total in waiting list: %d\n", q->count);
    pressEnter();
}

/* Displays all pending applications in the heap */
void viewPending(const HostelSystem *sys) {
    HDR("PENDING APPLICATIONS (Max-Heap)");

    if (sys->heap->size == 0) {
        printf("  No pending applications.\n"); BSEP(); pressEnter(); return;
    }
    for (int i = 0; i < sys->heap->size; i++) {
        Student *s = &sys->heap->data[i];
        printf("   [%2d] Enroll:%-12s  %-14s  Branch:%-5s  GPA:%.1f  Pri:%-3d \n",
               i + 1, s->enrollId, s->name, s->branch, s->gpa, s->priorityScore);
    }
    BSEP();
    printf("  Top priority: %s (Score: %d)\n",
           sys->heap->data[0].name, sys->heap->data[0].priorityScore);
    pressEnter();
}

/* Prints a branch-wise summary table: rooms, capacity, occupied, available */
void branchSummary(const HostelSystem *sys) {
    HDR("BRANCH-WISE ALLOCATION SUMMARY");

    const char *branches[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI,
                               BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD, BRANCH_ANY};
    int nb = (int)(sizeof(branches) / sizeof(branches[0]));

    printf("  %-6s | %-5s | %-5s | %-5s | %-8s\n",
           "Branch", "Rooms", "Cap", "Occup", "Avail");
    SEP();

    for (int b = 0; b < nb; b++) {
        int rooms = 0, cap = 0, occ = 0;
        RoomNode *cur = sys->rooms->head;
        while (cur) {
            if (strCaseEqual(cur->branch, branches[b])) {
                rooms++;
                cap += cur->capacity;
                occ += cur->occupancyCount;
            }
            cur = cur->next;
        }
        if (rooms > 0)
            printf("  %-6s | %-5d | %-5d | %-5d | %-5d\n",
                   branches[b], rooms, cap, occ, cap - occ);
    }
    BSEP();
    pressEnter();
}