#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hostel.h"

/* Allocates and initializes a new room node */
RoomNode *createRoom(int roomNumber, int capacity, const char *branch) {
    RoomNode *r = (RoomNode *)malloc(sizeof(RoomNode));
    if (!r) { fprintf(stderr, "FATAL: malloc RoomNode\n"); return NULL; }
    r->roomNumber     = roomNumber;
    r->capacity       = capacity;
    r->occupancyCount = 0;
    r->occupants      = NULL;
    r->prev = r->next = NULL;
    strncpy(r->branch, branch, MAX_BRANCH_LEN - 1);
    r->branch[MAX_BRANCH_LEN - 1] = '\0';
    strToUpper(r->branch);
    return r;
}

/* Appends a new room to the END of the doubly linked list */
int dllAppendRoom(RoomDLL *dll, int roomNumber, int capacity, const char *branch) {
    RoomNode *r = createRoom(roomNumber, capacity, branch);
    if (!r) return 0;
    if (!dll->head) {
        dll->head = dll->tail = r;
    } else {
        r->prev        = dll->tail;
        dll->tail->next = r;
        dll->tail       = r;
    }
    dll->size++;
    return 1;
}

/* Adds a student as an occupant at the END of the room's occupant list */
int addOccupant(RoomNode *room, Student s) {
    if (room->occupancyCount >= room->capacity) return 0;

    OccupantNode *n = (OccupantNode *)malloc(sizeof(OccupantNode));
    if (!n) { fprintf(stderr, "FATAL: malloc OccupantNode\n"); return 0; }
    n->student = s;
    n->next    = NULL;

    if (!room->occupants) {
        room->occupants = n;
    } else {
        OccupantNode *tail = room->occupants;
        while (tail->next) tail = tail->next;
        tail->next = n;
    }
    room->occupancyCount++;
    return 1;
}

/*
 * Returns 1 if a student's branch is compatible with a room's branch.
 * "ANY" on either side means it matches everything.
 */
int branchMatches(const char *roomBranch, const char *studentBranch) {
    if (strCaseEqual(roomBranch,    BRANCH_ANY)) return 1;
    if (strCaseEqual(studentBranch, BRANCH_ANY)) return 1;
    return strCaseEqual(roomBranch, studentBranch);
}

/*
 * Finds the best available room for a given branch.
 * "Best" = matching branch + has space + most occupied (bin-packing).
 */
RoomNode *findAvailableRoom(RoomDLL *dll, const char *branch) {
    RoomNode *best = NULL;
    RoomNode *cur  = dll->head;
    while (cur) {
        if (cur->occupancyCount < cur->capacity) {
            if (branchMatches(cur->branch, branch)) {
                if (!best || cur->occupancyCount > best->occupancyCount)
                    best = cur;
            }
        }
        cur = cur->next;
    }
    return best;
}

/* Frees all occupant nodes inside a single room */
void freeOccupants(RoomNode *room) {
    OccupantNode *cur = room->occupants;
    while (cur) {
        OccupantNode *t = cur->next;
        free(cur);
        cur = t;
    }
    room->occupants      = NULL;
    room->occupancyCount = 0;
}

/* Frees every room in the DLL along with their occupants */
void freeDLL(RoomDLL *dll) {
    RoomNode *cur = dll->head;
    while (cur) {
        RoomNode *t = cur->next;
        freeOccupants(cur);
        free(cur);
        cur = t;
    }
    dll->head = dll->tail = NULL;
    dll->size = 0;
}