#include <stdio.h>
#include <stdlib.h>
#include "hostel.h"

/* Checks if an enrollment ID already exists anywhere in the system */
int enrollIdExists(const HostelSystem *sys, const char *id) {
    /* Check all room occupants */
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseEqual(occ->student.enrollId, id)) return 1;
            occ = occ->next;
        }
        room = room->next;
    }
    /* Check pending heap */
    for (int i = 0; i < sys->heap->size; i++)
        if (strCaseEqual(sys->heap->data[i].enrollId, id)) return 1;

    /* Check waiting list */
    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, id)) return 1;
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }
    return 0;
}

/*
 * Initializes the entire hostel system:
 * - Allocates memory for DLL, Heap, CircularQueue
 * - Creates 6 default rooms (one per branch)
 */
HostelSystem *initSystem(void) {
    HostelSystem *sys = (HostelSystem *)malloc(sizeof(HostelSystem));
    if (!sys) return NULL;

    sys->rooms = (RoomDLL *)malloc(sizeof(RoomDLL));
    if (!sys->rooms) { free(sys); return NULL; }
    sys->rooms->head = sys->rooms->tail = NULL;
    sys->rooms->size = 0;

    sys->heap = (MaxHeap *)malloc(sizeof(MaxHeap));
    if (!sys->heap) { free(sys->rooms); free(sys); return NULL; }
    sys->heap->size = 0;

    sys->waitingList = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (!sys->waitingList) { free(sys->heap); free(sys->rooms); free(sys); return NULL; }
    sys->waitingList->front = sys->waitingList->rear = sys->waitingList->count = 0;

    sys->nextSysId      = 1;
    sys->nextRoomNumber = 101;

    /* Create 6 default rooms: one per branch */
    const char *branches[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI,
                               BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD};
    for (int i = 0; i < INITIAL_ROOMS; i++)
        dllAppendRoom(sys->rooms, sys->nextRoomNumber++, MAX_OCCUPANTS, branches[i]);

    printf("  [OK] %d default rooms created (CSE/ECE/AI/MCA/MTECH/PHD).\n", INITIAL_ROOMS);
    return sys;
}

/*
 * Frees ALL memory: every room, every occupant, heap, queue, and sys itself.
 * Called on exit — zero memory leaks.
 */
void destroySystem(HostelSystem *sys) {
    if (!sys) return;
    freeDLL(sys->rooms);
    free(sys->rooms);
    free(sys->heap);
    free(sys->waitingList);
    free(sys);
}