#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hostel.h"

/* Converts a string to UPPERCASE in-place */
void strToUpper(char *s) {
    for (; *s; s++) {
        if (*s >= 'a' && *s <= 'z') *s -= 32;
    }
}

/* Case-insensitive string equality: returns 1 if equal, 0 otherwise */
int strCaseEqual(const char *a, const char *b) {
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? (*a + 32) : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? (*b + 32) : *b;
        if (ca != cb) return 0;
        a++; b++;
    }
    return (*a == '\0' && *b == '\0');
}

/* Case-insensitive substring search: returns pointer to match or NULL */
const char *strCaseStr(const char *haystack, const char *needle) {
    if (!needle || !*needle) return haystack;

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
        if (match && strlen(haystack) >= needleLen) return haystack;
        haystack++;
    }
    return NULL;
}

/* Returns 1 if string contains only digits, 0 otherwise */
int isNumericOnly(const char *s) {
    if (!s || !*s) return 0;
    for (; *s; s++) {
        if (*s < '0' || *s > '9') return 0;
    }
    return 1;
}

/* Calculates student priority score: (GPA x 10) + (distance / 100) */
int computePriority(float gpa, int distanceKm) {
    return (int)(gpa * 10.0f) + (distanceKm / 100);
}

/* Clears leftover characters from stdin buffer */
void flushIn(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Reads an integer between lo and hi, keeps looping until valid */
int readInt(const char *prompt, int lo, int hi) {
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

/* Reads a full line of text (supports spaces in names) */
void readStr(const char *prompt, char *buf, int maxLen) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buf, maxLen, stdin)) {
        buf[strcspn(buf, "\n")] = '\0';
    } else {
        buf[0] = '\0';
    }
}

/* Reads and validates a branch name (CSE/ECE/AI/MCA/MTECH/PHD/ANY) */
int readBranch(const char *prompt, char *buf) {
    const char *valid[] = {BRANCH_CSE, BRANCH_ECE, BRANCH_AI,
                           BRANCH_MCA, BRANCH_MTECH, BRANCH_PHD, BRANCH_ANY};
    int n = (int)(sizeof(valid) / sizeof(valid[0]));

    printf("%s [CSE/ECE/AI/MCA/MTECH/PHD/ANY]: ", prompt);
    if (scanf(" %9s", buf) != 1) { flushIn(); return 0; }
    flushIn();
    strToUpper(buf);

    for (int i = 0; i < n; i++)
        if (strCaseEqual(buf, valid[i])) return 1;

    printf("  [!] Invalid branch. Must be CSE/ECE/AI/MCA/MTECH/PHD/ANY.\n");
    return 0;
}