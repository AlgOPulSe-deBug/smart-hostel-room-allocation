#include <stdio.h>
#include <stdlib.h>
#include "hostel.h"


int main(void) {
    /* Initialize system: create rooms, heap, waiting list */
    HostelSystem *sys = initSystem();
    if (!sys) {
        fprintf(stderr, "FATAL: Cannot initialise system.\n");
        return EXIT_FAILURE;
    }

    int choice;
    do {
        /* Show menu and read user choice */
        printMenu(sys);
        if (scanf("%d", &choice) != 1) {
            flushIn();
            choice = -1;
        } else {
            flushIn();
        }

        /* Dispatch to the correct feature function */
        switch (choice) {
            case  1: registerStudent(sys);   break;
            case  2: editStudent(sys);       break;
            case  3: searchStudent(sys);     break;
            case  4: deleteStudent(sys);     break;
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
                destroySystem(sys); /* Free all memory */
                printf("  Goodbye!\n\n");
                break;
            default:
                printf("  [!] Invalid choice.\n");
                pressEnter();
        }
    } while (choice != 0);

    return EXIT_SUCCESS;
}