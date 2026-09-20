#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_HEAP_SIZE    100
#define MAX_QUEUE_SIZE   50
#define MAX_NAME_LEN     64
#define MAX_BRANCH_LEN   10      
#define MAX_ENROLLID_LEN 20
#define MAX_OCCUPANTS    4
#define INITIAL_ROOMS    6       

#define BRANCH_CSE   "CSE"
#define BRANCH_ECE   "ECE"
#define BRANCH_AI    "AI"
#define BRANCH_MCA   "MCA"
#define BRANCH_MTECH "MTECH"
#define BRANCH_PHD   "PHD"
#define BRANCH_ANY   "ANY"

#define LINE60  "  +------------------------------------------------------------+"
#define LINE60E "  +============================================================+"
#define HDR(title) \
    do { \
        printf("\n%s\n", LINE60E); \
        printf("  |  %-58s|\n", title); \
        printf("%s\n", LINE60E); \
    } while(0)
#define SEP()  printf("%s\n", LINE60)
#define BSEP() printf("%s\n", LINE60E)


static void clearScreen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

static void pressEnter(void) {
    printf("\n  [ Press ENTER to continue... ]");
    fflush(stdout);
    int c; while ((c = getchar()) != '\n' && c != EOF);
}

static void strToUpper(char *s) {
    for (; *s; s++) {
        if (*s >= 'a' && *s <= 'z') *s -= 32;
    }
}

static int strCaseEqual(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? (*a + 32) : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? (*b + 32) : *b;
        if (ca != cb)
            return 0;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

static const char *strCaseStr(const char *haystack, const char *needle) {
    if (!needle || !*needle) 
        return haystack;
    
    size_t needleLen = strlen(needle);
    while (*haystack) {
        int match = 1;
        for (size_t i = 0; i < needleLen && haystack[i]; i++) {
            char ch = (haystack[i] >= 'A' && haystack[i] <= 'Z') 
                      ? (haystack[i] + 32) : haystack[i];
            char cn = (needle[i] >= 'A' && needle[i] <= 'Z') 
                      ? (needle[i] + 32) : needle[i];
            if (ch != cn) { match = 0; break; }
        }
        if (match && strlen(haystack) >= needleLen) 
            return haystack;
        haystack++;
    }
    return NULL;
}


static int isNumericOnly(const char *s) {
    if (!s || !*s) 
        return 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9') return 0;
    }
    return 1;
}

typedef struct Student {
    int   sysId;
    char  enrollId[MAX_ENROLLID_LEN];
    char  name[MAX_NAME_LEN];
    char  branch[MAX_BRANCH_LEN];
    float gpa;
    int   distanceKm;
    int   priorityScore;
} Student;


typedef struct OccupantNode {
    Student              student;
    struct OccupantNode *next;
} OccupantNode;

typedef struct RoomNode {
    int              roomNumber;
    int              capacity;
    int              occupancyCount;
    char             branch[MAX_BRANCH_LEN];
    OccupantNode    *occupants;
    struct RoomNode *prev;
    struct RoomNode *next;
} RoomNode;

typedef struct {
    RoomNode *head;
    RoomNode *tail;
    int       size;
} RoomDLL;

typedef struct {
    Student data[MAX_HEAP_SIZE];
    int     size;
} MaxHeap;

typedef struct {
    Student data[MAX_QUEUE_SIZE];
    int     front;
    int     rear;
    int     count;
} CircularQueue;

typedef struct {
    RoomDLL       *rooms;
    MaxHeap       *heap;
    CircularQueue *waitingList;
    int            nextSysId;
    int            nextRoomNumber;
} HostelSystem;

static int computePriority(float gpa, int distanceKm) {
    return (int)(gpa * 10.0f) + (distanceKm / 100);
}


static void heapSwap(Student *a, Student *b) {
    Student t = *a; *a = *b; *b = t;
}

static int heapHigher(const Student *a, const Student *b) {
    if (a->priorityScore != b->priorityScore)
        return a->priorityScore > b->priorityScore;
    return a->sysId < b->sysId;  
}

static void shiftUp(MaxHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heapHigher(&h->data[i], &h->data[p])) {
            heapSwap(&h->data[p], &h->data[i]);
            i = p;
        } 
        else 
            break;
    }
}

static void shiftDown(MaxHeap *h, int i) {
    int n = h->size;
    while (1) {
        int best = i, l = 2*i+1, r = 2*i+2;
        if (l < n && heapHigher(&h->data[l], &h->data[best])) 
            best = l;
        if (r < n && heapHigher(&h->data[r], &h->data[best])) 
            best = r;
        if (best != i) {
            heapSwap(&h->data[i], &h->data[best]);
            i = best;
        } 
        else   
            break;
    }
}

static int heapInsert(MaxHeap *h, Student s) {
    if (h->size >= MAX_HEAP_SIZE) {
        printf("  [!] Priority queue is full.\n"); return 0;
    }
    h->data[h->size] = s;
    shiftUp(h, h->size);
    h->size++;
    return 1;
}

static int heapExtractMax(MaxHeap *h, Student *out) {
    if (h->size == 0) 
        return 0;
    *out = h->data[0];
    h->size--;
    if (h->size > 0) { 
        h->data[0] = h->data[h->size]; shiftDown(h, 0); 
    }
    return 1;
}

static int heapRemoveAt(MaxHeap *h, int i) {
    if (i < 0 || i >= h->size) return 0;
    h->size--;
    if (i < h->size) {
        h->data[i] = h->data[h->size];
        shiftUp(h, i);
        shiftDown(h, i);
    }
    return 1;
}

static int queueIsFull (const CircularQueue *q){ 
    return q->count == MAX_QUEUE_SIZE;
}
static int queueIsEmpty(const CircularQueue *q){ 
    return q->count == 0; 
}

static int enqueue(CircularQueue *q, Student s) {
    if (queueIsFull(q)) { 
        printf("  [!] Waiting list is full.\n"); return 0; 
    }
    q->data[q->rear] = s;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->count++;
    return 1;
}

static int dequeue(CircularQueue *q, Student *out) {
    if (queueIsEmpty(q)) 
        return 0;
    *out     = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return 1;
}

static int queueRemoveAt(CircularQueue *q, int logicalIdx) {
    if (logicalIdx < 0 || logicalIdx >= q->count) 
        return 0;
    
    Student temp[MAX_QUEUE_SIZE];
    int newCount = 0;
    int p = q->front;
    
    for (int k = 0; k < q->count; k++) {
        if (k != logicalIdx)
            temp[newCount++] = q->data[p];
        p = (p + 1) % MAX_QUEUE_SIZE;
    }
    
    q->front = 0;
    q->rear  = newCount;
    q->count = newCount;
    memcpy(q->data, temp, newCount * sizeof(Student));
    return 1;
}

static RoomNode *createRoom(int roomNumber, int capacity,
                            const char *branch) {
    RoomNode *r = (RoomNode *)malloc(sizeof(RoomNode));
    if (!r) { fprintf(stderr, "FATAL: malloc RoomNode\n"); return NULL; }
    r->roomNumber     = roomNumber;
    r->capacity       = capacity;
    r->occupancyCount = 0;
    r->occupants      = NULL;
    r->prev = r->next = NULL;
    strncpy(r->branch, branch, MAX_BRANCH_LEN - 1);
    r->branch[MAX_BRANCH_LEN - 1] = '\0';
    strToUpper(r->branch);  /* [NEW-1] Store uppercase */
    return r;
}

static int dllAppendRoom(RoomDLL *dll, int roomNumber, int capacity,
                         const char *branch) {
    RoomNode *r = createRoom(roomNumber, capacity, branch);
    if (!r) return 0;
    if (!dll->head) { dll->head = dll->tail = r; }
    else { r->prev = dll->tail; dll->tail->next = r; dll->tail = r; }
    dll->size++;
    return 1;
}

static int addOccupant(RoomNode *room, Student s) {
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

static int branchMatches(const char *roomBranch, const char *studentBranch) {
    if (strCaseEqual(roomBranch, BRANCH_ANY)) return 1;
    if (strCaseEqual(studentBranch, BRANCH_ANY)) return 1;
    return strCaseEqual(roomBranch, studentBranch);
}

static RoomNode *findAvailableRoom(RoomDLL *dll, const char *branch) {
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

static void freeOccupants(RoomNode *room) {
    OccupantNode *cur = room->occupants;
    while (cur) { OccupantNode *t = cur->next; free(cur); cur = t; }
    room->occupants      = NULL;
    room->occupancyCount = 0;
}

static void freeDLL(RoomDLL *dll) {
    RoomNode *cur = dll->head;
    while (cur) { RoomNode *t = cur->next; freeOccupants(cur); free(cur); cur = t; }
    dll->head = dll->tail = NULL; dll->size = 0;
}

static HostelSystem *initSystem(void) {
    HostelSystem *sys = (HostelSystem *)malloc(sizeof(HostelSystem));
    if (!sys) 
        return NULL;

    sys->rooms = (RoomDLL *)malloc(sizeof(RoomDLL));
    if (!sys->rooms) {
        free(sys); 
            return NULL; 
    }
    sys->rooms->head = sys->rooms->tail = NULL; sys->rooms->size = 0;

    sys->heap = (MaxHeap *)malloc(sizeof(MaxHeap));
    if (!sys->heap) { 
        free(sys->rooms); 
        free(sys); 
            return NULL; 
    }
    sys->heap->size = 0;

    sys->waitingList = (CircularQueue *)malloc(sizeof(CircularQueue));
    if (!sys->waitingList) { 
        free(sys->heap); 
        free(sys->rooms); 
        free(sys); 
            return NULL; 
    }
    sys->waitingList->front = sys->waitingList->rear = sys->waitingList->count = 0;

    sys->nextSysId      = 1;
    sys->nextRoomNumber = 101;

    const char *branches[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI, 
                               BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD};
    for (int i = 0; i < INITIAL_ROOMS; i++)
        dllAppendRoom(sys->rooms, sys->nextRoomNumber++,
                      MAX_OCCUPANTS, branches[i]);
    printf("  [OK] %d default rooms created (CSE/ECE/AI/MCA/MTECH/PHD).\n",
           INITIAL_ROOMS);

    return sys;
}

static void destroySystem(HostelSystem *sys) {
    if (!sys) return;
    freeDLL(sys->rooms);
    free(sys->rooms);
    free(sys->heap);
    free(sys->waitingList);
    free(sys);
}

static void flushIn(void) { int c; while ((c = getchar()) != '\n' && c != EOF); }

static int readInt(const char *prompt, int lo, int hi) {
    int v;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &v) == 1 && v >= lo && v <= hi) { 
            flushIn(); 
                return v; 
        }
        printf("  [!] Enter a number between %d and %d.\n", lo, hi);
        flushIn();
    }
}

static void readStr(const char *prompt, char *buf, int maxLen) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, maxLen, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    } 
    else {
        buf[0] = '\0';
    }
}

static int readBranch(const char *prompt, char *buf) {
    const char *valid[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI,
                           BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD, BRANCH_ANY};
    int n = (int)(sizeof(valid)/sizeof(valid[0]));
    
    printf("%s [CSE/ECE/AI/MCA/MTECH/PHD/ANY]: ", prompt);
    if (scanf(" %9s", buf) != 1) { 
        flushIn();
            return 0; 
    }
    flushIn();
    strToUpper(buf);  
    
    for (int i = 0; i < n; i++)
        if (strCaseEqual(buf, valid[i])) 
            return 1;
    
    printf("  [!] Invalid branch. Must be CSE/ECE/AI/MCA/MTECH/PHD/ANY.\n");
    return 0;
}

static int enrollIdExists(const HostelSystem *sys, const char *id) {
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseEqual(occ->student.enrollId, id)) 
                return 1;
            occ = occ->next;
        }
        room = room->next;
    }
    for (int i = 0; i < sys->heap->size; i++)
        if (strCaseEqual(sys->heap->data[i].enrollId, id)) 
            return 1;

    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, id)) 
            return 1;
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }
    return 0;
}

static void registerStudent(HostelSystem *sys) {
    HDR("REGISTER NEW STUDENT");

    Student s;
    memset(&s, 0, sizeof(s));
    s.sysId = sys->nextSysId++;

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
            printf("  [!] Enrollment ID \"%s\" already exists in the system.\n", s.enrollId); 
            continue;
        }
        break;
    }

    do {
        readStr("  Full Name      : ", s.name, MAX_NAME_LEN);
        if (strlen(s.name) == 0)
            printf("  [!] Name cannot be empty.\n");
    } while (strlen(s.name) == 0);

    while (!readBranch("  Branch", s.branch));
    strToUpper(s.branch);  /* Ensure uppercase storage */

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


static int editStudentFields(Student *s) {
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
        case 4: {
            int tmp = readInt("  New Distance (km): ", 0, 99999);
            s->distanceKm = tmp;
            break;
        }
        case 0:
            return 0;
    }
    s->priorityScore = computePriority(s->gpa, s->distanceKm);
    return 1;
}

static void editStudent(HostelSystem *sys) {
    HDR("EDIT STUDENT DETAILS");

    char target[MAX_ENROLLID_LEN];
    printf("  Enter Enrollment No to edit: ");
    if (scanf(" %19s", target) != 1) { 
        flushIn(); 
        return; 
    }
    flushIn();

    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseEqual(occ->student.enrollId, target)) {
                if (editStudentFields(&occ->student)) {
                    printf("  [OK] Student updated (Room %d).\n", room->roomNumber);
                }
                pressEnter(); 
                return;
            }
            occ = occ->next;
        }
        room = room->next;
    }

    for (int i = 0; i < sys->heap->size; i++) {
        if (strCaseEqual(sys->heap->data[i].enrollId, target)) {
            if (editStudentFields(&sys->heap->data[i])) {
                shiftUp(sys->heap, i);
                shiftDown(sys->heap, i);
                printf("  [OK] Student updated (pending queue).\n");
            }
            pressEnter(); 
            return;
        }
    }

    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, target)) {
            if (editStudentFields(&sys->waitingList->data[pos])) {
                printf("  [OK] Student updated (waiting list).\n");
            }
            pressEnter(); 
            return;
        }
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }

    printf("  [!] Enrollment No \"%s\" not found.\n", target);
    pressEnter();
}

static void deleteStudent(HostelSystem *sys) {
    HDR("DELETE STUDENT FROM SYSTEM");

    char target[MAX_ENROLLID_LEN];
    printf("  Enter Enrollment No to delete: ");
    if (scanf(" %19s", target) != 1) { flushIn(); pressEnter(); return; }
    flushIn();

    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *cur  = room->occupants;
        OccupantNode *prev = NULL;
        while (cur) {
            if (strCaseEqual(cur->student.enrollId, target)) {
                printf("\n  Found: %s (%s) in Room %d\n", 
                       cur->student.name, cur->student.enrollId, room->roomNumber);
                printf("  Are you sure you want to delete? (1=Yes, 0=No): ");
                int confirm = readInt("", 0, 1);
                
                if (confirm == 1) {
                    if (prev) 
                        prev->next = cur->next;
                    else       
                        room->occupants = cur->next;
                    printf("  [OK] %s (%s) deleted from Room %d.\n",
                           cur->student.name, cur->student.enrollId, room->roomNumber);
                    free(cur);
                    room->occupancyCount--;
                } 
                else {
                    printf("  [!] Deletion cancelled.\n");
                }
                pressEnter();
                return;
            }
            prev = cur; cur = cur->next;
        }
        room = room->next;
    }

    for (int i = 0; i < sys->heap->size; i++) {
        if (strCaseEqual(sys->heap->data[i].enrollId, target)) {
            printf("\n  Found: %s (%s) in Pending Applications\n", 
                   sys->heap->data[i].name, sys->heap->data[i].enrollId);
            printf("  Are you sure you want to delete? (1=Yes, 0=No): ");
            int confirm = readInt("", 0, 1);
            
            if (confirm == 1) {
                printf("  [OK] %s (%s) deleted from pending queue.\n",
                       sys->heap->data[i].name, sys->heap->data[i].enrollId);
                heapRemoveAt(sys->heap, i);
            } 
            else {
                printf("  [!] Deletion cancelled.\n");
            }
            pressEnter();
            return;
        }
    }

    int pos = sys->waitingList->front;
    for (int i = 0; i < sys->waitingList->count; i++) {
        if (strCaseEqual(sys->waitingList->data[pos].enrollId, target)) {
            printf("\n  Found: %s (%s) in Waiting List\n", 
                   sys->waitingList->data[pos].name, 
                   sys->waitingList->data[pos].enrollId);
            printf("  Are you sure you want to delete? (1=Yes, 0=No): ");
            int confirm = readInt("", 0, 1);
            
            if (confirm == 1) {
                printf("  [OK] %s (%s) deleted from waiting list.\n",
                       sys->waitingList->data[pos].name,
                       sys->waitingList->data[pos].enrollId);
                queueRemoveAt(sys->waitingList, i);
            } else {
                printf("  [!] Deletion cancelled.\n");
            }
            pressEnter();
            return;
        }
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }

    printf("  [!] Enrollment No \"%s\" not found in system.\n", target);
    pressEnter();
}

/* ================================================================
 *  ALLOCATION ENGINE
 * ================================================================ */
static void allocateOne(HostelSystem *sys) {
    if (sys->heap->size == 0) {
        printf("  [i] No pending applications.\n"); return;
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

static void allocateNextMenu(HostelSystem *sys) {
    HDR("ALLOCATE NEXT APPLICANT");
    allocateOne(sys);
    pressEnter();
}

static void allocateAllMenu(HostelSystem *sys) {
    HDR("ALLOCATE ALL PENDING APPLICANTS");
    if (sys->heap->size == 0) {
        printf("  [i] Priority queue is empty.\n");
    } else {
        while (sys->heap->size > 0) allocateOne(sys);
    }
    pressEnter();
}

/* ================================================================
 *  FEATURE: VACATE ROOM (removes from room, promotes from waitlist)
 * ================================================================ */
static void vacateRoom(HostelSystem *sys) {
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
                if (prev) prev->next = cur->next;
                else       room->occupants = cur->next;
                printf("  [OK] %s (%s) removed from Room %d.\n",
                       cur->student.name, cur->student.enrollId,
                       room->roomNumber);
                free(cur);
                room->occupancyCount--;

                /* [FIX-1] Promote from waiting list */
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
                            (sys->waitingList->front + targetIdx)
                            % MAX_QUEUE_SIZE];

                        queueRemoveAt(sys->waitingList, targetIdx);

                        addOccupant(room, promoted);
                        printf("  [PROMOTED] %s moved from waiting list"
                               " to Room %d.\n",
                               promoted.name, room->roomNumber);
                    }
                }

                pressEnter();
                return;
            }
            prev = cur; cur = cur->next;
        }
        room = room->next;
    }
    printf("  [!] Enrollment No \"%s\" not found in any room.\n", target);
    pressEnter();
}

/* ================================================================
 *  VIEW ROOM STATUS
 * ================================================================ */
static const char *statusTag(const RoomNode *r) {
    if (r->occupancyCount == 0)           return "VACANT  ";
    if (r->occupancyCount == r->capacity) return "FULL    ";
    return "PARTIAL ";
}

static void viewRooms(const HostelSystem *sys) {
    HDR("ROOM STATUS (Doubly Linked List)");

    RoomNode *cur = sys->rooms->head;
    if (!cur) { printf("  No rooms registered.\n"); pressEnter(); return; }

    while (cur) {
        printf("  | Room %-3d  Branch: %-5s  [%d/%d]  Status: %s  |\n",
               cur->roomNumber, cur->branch,
               cur->occupancyCount, cur->capacity, statusTag(cur));
        SEP();

        OccupantNode *occ = cur->occupants;
        if (!occ) printf("  |   (vacant)                                              |\n");
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

/* ================================================================
 *  VIEW WAITING LIST
 * ================================================================ */
static void viewWaitingList(const HostelSystem *sys) {
    HDR("WAITING LIST (Circular Queue)");

    const CircularQueue *q = sys->waitingList;
    if (queueIsEmpty(q)) {
        printf("  Waiting list is empty.\n"); BSEP(); pressEnter(); return;
    }

    int pos = q->front;
    for (int i = 0; i < q->count; i++) {
        printf("  | #%-3d  Enroll:%-12s  %-16s  Branch:%-5s  Pri:%-3d |\n",
               i+1,
               q->data[pos].enrollId, q->data[pos].name,
               q->data[pos].branch,   q->data[pos].priorityScore);
        pos = (pos + 1) % MAX_QUEUE_SIZE;
    }
    BSEP();
    printf("  Total in waiting list: %d\n", q->count);
    pressEnter();
}

/* ================================================================
 *  VIEW PENDING APPLICATIONS (Heap)
 * ================================================================ */
static void viewPending(const HostelSystem *sys) {
    HDR("PENDING APPLICATIONS (Max-Heap)");

    if (sys->heap->size == 0) {
        printf("  No pending applications.\n"); BSEP(); pressEnter(); return;
    }
    for (int i = 0; i < sys->heap->size; i++) {
        Student *s = &sys->heap->data[i];
        printf("  | [%2d] Enroll:%-12s  %-14s  Branch:%-5s"
               "  GPA:%.1f  Pri:%-3d |\n",
               i+1, s->enrollId, s->name, s->branch,
               s->gpa, s->priorityScore);
    }
    BSEP();
    printf("  Top priority: %s (Score: %d)\n",
           sys->heap->data[0].name,
           sys->heap->data[0].priorityScore);
    pressEnter();
}

/* ================================================================
 *  [v2.3] AUTO-ALLOCATE: scan waiting list into a newly added room
 *  Called after every dllAppendRoom so waitlisted students get
 *  placed without any manual step.
 * ================================================================ */
static void autoAllocateWaitlist(HostelSystem *sys) {
    if (queueIsEmpty(sys->waitingList)) return;

    RoomNode *room = sys->rooms->tail;   /* the room just appended */
    int promoted = 0;

    while (room->occupancyCount < room->capacity &&
           !queueIsEmpty(sys->waitingList)) {
        /* Find the first waiting student whose branch matches */
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
        if (targetIdx < 0) break;   /* No matching student */

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

/* ================================================================
 *  ADD NEW ROOM
 * ================================================================ */
static void addNewRoom(HostelSystem *sys) {
    HDR("ADD NEW ROOM");
    char branch[MAX_BRANCH_LEN];
    while (!readBranch("  Branch for this room", branch));
    int cap = readInt("  Capacity (1-8): ", 1, 8);
    if (dllAppendRoom(sys->rooms, sys->nextRoomNumber, cap, branch)) {
        printf("  [OK] Room %d (Branch: %s, Capacity: %d) added.\n",
               sys->nextRoomNumber, branch, cap);
        sys->nextRoomNumber++;
        /* [v2.3] Auto-allocate any matching students from waiting list */
        autoAllocateWaitlist(sys);
    }
    pressEnter();
}

/* ================================================================
 *  SEARCH STUDENT (case-insensitive)
 * ================================================================ */
static void searchStudent(const HostelSystem *sys) {
    HDR("SEARCH STUDENT");
    char query[MAX_NAME_LEN];
    readStr("  Name or Enrollment No (partial ok): ", query, MAX_NAME_LEN);

    /* [FIX-5] Reject empty query */
    if (strlen(query) == 0) {
        printf("  [!] Search term cannot be empty.\n");
        pressEnter();
        return;
    }

    int found = 0;

    /* [NEW-1] Case-insensitive search */
    RoomNode *room = sys->rooms->head;
    while (room) {
        OccupantNode *occ = room->occupants;
        while (occ) {
            if (strCaseStr(occ->student.name,     query) ||
                strCaseStr(occ->student.enrollId, query)) {
                printf("  [Room %3d] %-12s  %-18s  Branch:%-5s  GPA:%.1f  Pri:%d\n",
                       room->roomNumber, occ->student.enrollId,
                       occ->student.name, occ->student.branch,
                       occ->student.gpa,  occ->student.priorityScore);
                found++;
            }
            occ = occ->next;
        }
        room = room->next;
    }

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

/* ================================================================
 *  BRANCH-WISE SUMMARY
 * ================================================================ */
static void branchSummary(const HostelSystem *sys) {
    HDR("BRANCH-WISE ALLOCATION SUMMARY");

    /* [NEW-2] Updated branches */
    const char *branches[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI,
                               BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD, BRANCH_ANY};
    int nb = (int)(sizeof(branches)/sizeof(branches[0]));

    printf("  %-6s | %-5s | %-5s | %-5s | %-8s\n",
           "Branch","Rooms","Cap","Occup","Avail");
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

/* ================================================================
 *  MAIN MENU
 * ================================================================ */
static void printBanner(void) {
    printf("\n");
    printf("  +------------------------------------------------------------+\n");
    printf("  |                                                            |\n");
    printf("  |       SMART HOSTEL MANAGEMENT SYSTEM  v2.3                 |\n");
    printf("  |       Branch-Aware | Priority-Driven | Persistent          |\n");
    printf("  |       Branches: CSE/ECE/AI/MCA/MTECH/PHD                   |\n");
    printf("  |                                                            |\n");
    printf("  +------------------------------------------------------------+\n");
}

static void printMenu(const HostelSystem *sys) {
    clearScreen();
    printBanner();
    printf("  | Rooms: %-3d  |  Pending: %-3d  |  Waiting: %-3d            |\n",
           sys->rooms->size, sys->heap->size, sys->waitingList->count);
    printf("  +------------------------------------------------------------+\n");
    printf("  |                                                            |\n");
    printf("  |  STUDENTS                                                  |\n");
    printf("  |   1. Register Student                                      |\n");
    printf("  |   2. Edit Student Details                                  |\n");
    printf("  |   3. Search Student                                        |\n");
    printf("  |   4. Delete Student                                        |\n");
    printf("  |                                                            |\n");
    printf("  |  ALLOCATION                                                |\n");
    printf("  |   5. Allocate Next Applicant                               |\n");
    printf("  |   6. Allocate All Pending                                  |\n");
    printf("  |   7. Vacate Room (Remove & Promote)                        |\n");
    printf("  |                                                            |\n");
    printf("  |  REPORTS                                                   |\n");
    printf("  |   8. View Room Status                                      |\n");
    printf("  |   9. View Waiting List                                     |\n");
    printf("  |  10. View Pending Applications                             |\n");
    printf("  |  11. Branch-Wise Summary                                   |\n");
    printf("  |                                                            |\n");
    printf("  |  SYSTEM                                                    |\n");
    printf("  |  12. Add New Room                                          |\n");
    printf("  |   0. Exit                                                  |\n");
    printf("  |                                                            |\n");
    printf("  |  [TIP] Enter 0 from any sub-menu to return here.          |\n");
    printf("  +------------------------------------------------------------+\n");
    printf("  Choice: ");
}



/* ================================================================
 *  ENTRY POINT
 * ================================================================ */
int main(void) {
    HostelSystem *sys = initSystem();
    if (!sys) {
        fprintf(stderr, "FATAL: Cannot initialise system.\n");
        return EXIT_FAILURE;
    }

    int choice;
    do {
        printMenu(sys);
        if (scanf("%d", &choice) != 1) {
            flushIn(); choice = -1;
        } else {
            flushIn();
        }

        switch (choice) {
            case  1: registerStudent(sys);   break;
            case  2: editStudent(sys);       break;
            case  3: searchStudent(sys);     break;
            case  4: deleteStudent(sys);     break;      /* [NEW-3] */
            case  5: allocateNextMenu(sys);  break;
            case  6: allocateAllMenu(sys);   break;
            case  7: vacateRoom(sys);        break;
            case  8: viewRooms(sys);         break;
            case  9: viewWaitingList(sys);   break;
            case 10: viewPending(sys);       break;
            case 11: branchSummary(sys);     break;
            case 12: addNewRoom(sys);        break;
            case 0:
                clearScreen();
                destroySystem(sys);
                printf("  Goodbye!\n\n");
                break;
            default:
                printf("  [!] Invalid choice.\n");
                pressEnter();
        }
    } while (choice != 0);

    return EXIT_SUCCESS;
}