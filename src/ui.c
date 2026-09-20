#include <stdio.h>
#include "hostel.h"

/* Clears terminal using ANSI escape code */
void clearScreen(void) {
    printf("\033[2J\033[H");
    fflush(stdout);
}

/* Waits for user to press Enter */
void pressEnter(void) {
    printf("\n  [ Press ENTER to continue... ]");
    fflush(stdout);
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/* Prints the big title banner */
void printBanner(void) {
    printf("\n");
    printf("          SMART HOSTEL MANAGEMENT SYSTEM                      \n");
}

/* Prints the full main menu with live room/pending/waiting counts */
void printMenu(const HostelSystem *sys) {
    clearScreen();
    printBanner();
    printf("   Rooms: %-3d  |  Pending: %-3d  |  Waiting: %-3d            \n",
           sys->rooms->size, sys->heap->size, sys->waitingList->count);
    printf("    STUDENTS                                                  \n");
    printf("     1. Register Student                                      \n");
    printf("     2. Edit Student Details                                  \n");
    printf("     3. Search Student                                        \n");
    printf("     4. Delete Student                                        \n");
    printf("\n");
    printf("    ALLOCATION                                                \n");
    printf("     5. Allocate Next Applicant                               \n");
    printf("     6. Allocate All Pending                                  \n");
    printf("     7. Vacate Room (Remove & Promote)                        \n");
    printf("\n");
    printf("    REPORTS                                                   \n");
    printf("     8. View Room Status                                      \n");
    printf("     9. View Waiting List                                     \n");
    printf("    10. View Pending Applications                             \n");
    printf("    11. Branch-Wise Summary                                   \n");
    printf("                                                              \n");
    printf("    SYSTEM                                                    \n");
    printf("    12. Add New Room                                          \n");
    printf("     0. Exit                                                  \n");
    printf("\n");
    printf("     Enter 0 from any sub-menu to return here.          \n");
    printf("\n");
    printf("  Choice: ");
}