#ifndef HOSTEL_H
#define HOSTEL_H

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
//structure
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

void clearScreen(void);
void pressEnter(void);
void printBanner(void);
void printMenu(const HostelSystem *sys);


void        strToUpper(char *s);
int         strCaseEqual(const char *a, const char *b);
const char *strCaseStr(const char *haystack, const char *needle);
int         isNumericOnly(const char *s);
int         computePriority(float gpa, int distanceKm);
void        flushIn(void);
int         readInt(const char *prompt, int lo, int hi);
void        readStr(const char *prompt, char *buf, int maxLen);
int         readBranch(const char *prompt, char *buf);

void heapSwap(Student *a, Student *b);
int  heapHigher(const Student *a, const Student *b);
void shiftUp(MaxHeap *h, int i);
void shiftDown(MaxHeap *h, int i);
int  heapInsert(MaxHeap *h, Student s);
int  heapExtractMax(MaxHeap *h, Student *out);
int  heapRemoveAt(MaxHeap *h, int i);

int queueIsFull(const CircularQueue *q);
int queueIsEmpty(const CircularQueue *q);
int enqueue(CircularQueue *q, Student s);
int dequeue(CircularQueue *q, Student *out);
int queueRemoveAt(CircularQueue *q, int logicalIdx);

RoomNode *createRoom(int roomNumber, int capacity, const char *branch);
int     dllAppendRoom(RoomDLL *dll, int roomNumber, int capacity, const char *branch);
int       addOccupant(RoomNode *room, Student s);
int       branchMatches(const char *roomBranch, const char *studentBranch);
RoomNode *findAvailableRoom(RoomDLL *dll, const char *branch);
void      freeOccupants(RoomNode *room);
void      freeDLL(RoomDLL *dll);


HostelSystem *initSystem(void);
void          destroySystem(HostelSystem *sys);
int           enrollIdExists(const HostelSystem *sys, const char *id);


void registerStudent(HostelSystem *sys);
int  editStudentFields(Student *s);
void editStudent(HostelSystem *sys);
void deleteStudent(HostelSystem *sys);
void searchStudent(const HostelSystem *sys);


void allocateOne(HostelSystem *sys);
void allocateNextMenu(HostelSystem *sys);
void allocateAllMenu(HostelSystem *sys);
void vacateRoom(HostelSystem *sys);
void autoAllocateWaitlist(HostelSystem *sys);
void addNewRoom(HostelSystem *sys);


const char *statusTag(const RoomNode *r);
void        viewRooms(const HostelSystem *sys);
void        viewWaitingList(const HostelSystem *sys);
void        viewPending(const HostelSystem *sys);
void        branchSummary(const HostelSystem *sys);

#endif /* HOSTEL_H */