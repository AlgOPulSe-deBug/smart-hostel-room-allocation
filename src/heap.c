#include <stdio.h>
#include "hostel.h"

// Swaps two students in the heap array 
void heapSwap(Student *a, Student *b) {
    Student t = *a;
    *a = *b;
    *b = t;
}

//Returns 1 if student 'a' has higher priority than 'b'

int heapHigher(const Student *a, const Student *b) {
    if (a->priorityScore != b->priorityScore)
        return a->priorityScore > b->priorityScore;
    return a->sysId < b->sysId;
}

//Bubbles student UP from index i to correct position 
void shiftUp(MaxHeap *h, int i) {
    while (i > 0) {
        int p = (i - 1) / 2;
        if (heapHigher(&h->data[i], &h->data[p])) {
            heapSwap(&h->data[p], &h->data[i]);
            i = p;
        } else break;
    }
}

// Pushes student DOWN from index i to correct position 
void shiftDown(MaxHeap *h, int i) {
    int n = h->size;
    while (1) {
        int best = i, l = 2 * i + 1, r = 2 * i + 2;
        if (l < n && heapHigher(&h->data[l], &h->data[best])) best = l;
        if (r < n && heapHigher(&h->data[r], &h->data[best])) best = r;
        if (best != i) {
            heapSwap(&h->data[i], &h->data[best]);
            i = best;
        } else break;
    }
}

// Inserts a student into the heap. Returns 1 on success, 0 if full 
int heapInsert(MaxHeap *h, Student s) {
    if (h->size >= MAX_HEAP_SIZE) {
        printf("  [!] Priority queue is full.\n");
        return 0;
    }
    h->data[h->size] = s;
    shiftUp(h, h->size);
    h->size++;
    return 1;
}

// Removes and returns the highest priority student. Returns 1 on success 
int heapExtractMax(MaxHeap *h, Student *out) {
    if (h->size == 0) return 0;
    *out = h->data[0];
    h->size--;
    if (h->size > 0) {
        h->data[0] = h->data[h->size];
        shiftDown(h, 0);
    }
    return 1;
}

// Removes student at a specific index i. Returns 1 on success      
int heapRemoveAt(MaxHeap *h, int i) {
    if (i < 0 || i >= h->size) return 0;
    h->size--;
    if (i < h->size) {
        h->data[i] = h->data[h->size];
        shiftUp(h, i);
        shiftDown(h, i);
    }
    return 1;
}