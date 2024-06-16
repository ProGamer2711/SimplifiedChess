#include "Menu.h"

#include <stdio.h>

void clearScreen() {
    system(CLEAR_COMMAND);
}

void waitForEnter() {
    // clear the input buffer
    getchar();

    printf("Press Enter to continue...");
    while (getchar() != '\n');
}

void printMainMenu() {
    printf("1. New game\n");
    printf("2. Change board size\n");
    printf("3. Replay a game\n");
    printf("4. Exit\n");
}

void printReplayMenu() {
    printf("1. To begining of game\n");
    printf("2. Forward\n");
    printf("3. Backwards\n");
    printf("4. To end of game\n");
    printf("5. Exit\n");
}

void runMenu(PrintMenuFunction printMenu, ExecuteSelectionFunction executeSelection) {
    unsigned short currentChoice = 0;

    while (1) {
        clearScreen();

        printMenu();

        printf("> ");
        scanf("%hu", &currentChoice);

        int result = executeSelection(currentChoice);

        if(result == 0) return;
    }
}