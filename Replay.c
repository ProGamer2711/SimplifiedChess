#include "Replay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Constants.h"
#include "Menu.h"
#include "Vector.h"

#define MOVE_SIZE 5

// 1 byte for board size
// 1 byte for piece count
// 2 bytes for each piece's position
byte calculateSeedLength(byte pieceCount) {
    return 2 + pieceCount * 2;
}

void writeMoveToFile(FILE *file, Move *move) {
    PieceType pieceTakenType = move->pieceTaken == NULL ? 0 : move->pieceTaken->type;
    byte pieceTaken = pieceTakenType;

    // a move can be represented by 5 bytes
    // 2 bytes for the start position
    // 2 bytes for the end position
    // 1 byte for the piece taken (the byte 0 if no piece was taken)
    byte moveBytes[MOVE_SIZE] = {
        move->start.x,
        move->start.y,
        move->end.x,
        move->end.y,
        pieceTaken,
    };

    if (fwrite(moveBytes, sizeof(byte), MOVE_SIZE, file) != MOVE_SIZE) {
        printf("Failed to write move to file");
        fclose(file);

        exit(1);
    }
}

void writeReplayToFile(byte *seed, size_t seedLength, Vector *moves) {
    getchar();

    printf("Where do you want to store your replay?\n> ");

    char fileName[MAX_FILE_NAME_LENGTH];
    fgets(fileName, MAX_FILE_NAME_LENGTH, stdin);

    // remove the \n from the end of the string
    fileName[strlen(fileName) - 1] = '\0';

    FILE *file = fopen(fileName, "wb");

    if (file == NULL) {
        printf("Failed to open file");

        exit(1);
    }

    if (fwrite(seed, sizeof(byte), seedLength, file) != seedLength) {
        printf("Failed to write seed to file");
        fclose(file);

        exit(1);
    }

    for (byte i = 0; i < moves->length; i++) {
        writeMoveToFile(file, moves->get(moves, i));
    }

    fclose(file);
}

void replayGame() {
    clearScreen();

    printf("Which file do you want to replay from?\n> ");

    char fileName[MAX_FILE_NAME_LENGTH];
    fgets(fileName, MAX_FILE_NAME_LENGTH, stdin);

    // remove the \n from the end of the string
    fileName[strlen(fileName) - 1] = '\0';

    // ! Nathaniel go brr...
}