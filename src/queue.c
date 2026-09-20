#include <stdio.h>
#include <string.h>
#include "hostel.h"

/* Returns 1 if waiting list has reached max capacity */
int queueIsFull(const CircularQueue *q) {
    return q->count == MAX_QUEUE_SIZE;
}

/* Returns 1 if waiting list is empty */
int queueIsEmpty(const CircularQueue *q) {
    return q->count == 0;
}

/* Adds student to the REAR of the waiting list (wrap-around) */
int enqueue(CircularQueue *q, Student s) {
    if (queueIsFull(q)) {
        printf("  [!] Waiting list is full.\n");
        return 0;
    }
    q->data[q->rear] = s;
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->count++;
    return 1;
}

/* Removes student from the FRONT of the waiting list (wrap-around) */
int dequeue(CircularQueue *q, Student *out) {
    if (queueIsEmpty(q)) return 0;
    *out     = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->count--;
    return 1;
}

/*
 * Removes student at a specific logical index (0 = front).
 * Used when promoting a branch-matching student from the middle of queue.
 * Rebuilds the queue into a temp array skipping the removed student.
 */
int queueRemoveAt(CircularQueue *q, int logicalIdx) {
    if (logicalIdx < 0 || logicalIdx >= q->count) return 0;

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