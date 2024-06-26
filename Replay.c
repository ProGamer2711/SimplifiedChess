#include "Replay.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Chess.h"
#include "Constants.h"
#include "Menu.h"
#include "Vector.h"

// 1 byte for board size
// 1 byte for piece count
// 2 bytes for each piece's position
byte calculateSeedLength(byte pieceCount) {
    return 2 + pieceCount * 2;
}

byte hasNextMove(FILE* file) {
    long currentPos = ftell(file);

    byte buffer[MOVE_SIZE];
    size_t bytesRead = fread(buffer, sizeof(byte), MOVE_SIZE, file);

    // Reset file position to original
    fseek(file, currentPos, SEEK_SET);

    return bytesRead == MOVE_SIZE;
}

static void writeMoveToFile(FILE* file, Move* move) {
    // a move can be represented by 4 bytes
    // 2 bytes for the start position
    // 2 bytes for the end position
    byte moveBytes[MOVE_SIZE] = {
        move->start.x,
        move->start.y,
        move->end.x,
        move->end.y,
    };

    if (fwrite(moveBytes, sizeof(byte), MOVE_SIZE, file) != MOVE_SIZE) {
        printf("Failed to write move to file");
        fclose(file);

        exit(1);
    }
}

void writeReplayToFile(byte* seed, size_t seedLength, Vector* moves) {
    getchar();

    printf("Where do you want to store your replay?\n> ");

    char fileName[MAX_FILE_NAME_LENGTH];
    fgets(fileName, MAX_FILE_NAME_LENGTH, stdin);

    // remove the \n from the end of the string
    fileName[strlen(fileName) - 1] = '\0';

    FILE* file = fopen(fileName, "wb");

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

void freeReplayArgument(Vector* replayArgument) {
    for (byte i = 0; i < replayArgument->length; i++) {
        free(replayArgument->get(replayArgument, i));
    }

    freeVector(replayArgument);
}

void freeReplayArguments(Vector* board, Vector* pieces, Vector* moves) {
    freeBoard(board);
    freeReplayArgument(pieces);
    freeReplayArgument(moves);
}

static void goToStartOfGame(FILE* file, Vector* pieceStartingPositions, Vector** board, Vector** pieces, Vector** moves) {
    byte offset = calculateSeedLength((*pieces)->length), boardSize = (*board)->length;

    fseek(file, offset, SEEK_SET);

    freeReplayArguments(*board, *pieces, *moves);

    initializeReplay(boardSize, pieceStartingPositions, board, pieces, moves);
}

static void goOneMoveBackward(FILE* file, Vector* board, Vector* pieces, Vector* moves) {
    long positionInFile = ftell(file);

    if (positionInFile > calculateSeedLength(pieces->length)) {
        fseek(file, -MOVE_SIZE, SEEK_CUR);
        undoMove(moves, board);
    }
}

static void goOneMoveForward(FILE* file, Vector* board, Vector* moves) {
    if (!hasNextMove(file)) {
        printf("No more moves to read\n");
        waitForEnter();

        return;
    }

    byte position[2];
    fread(position, sizeof(byte), 2, file);
    Tile* tile = getTileFromBoard(board, position[0], position[1]);

    fread(position, sizeof(byte), 2, file);
    Coordinate endCoordinate = {position[0], position[1]};

    makeMove(moves, board, tile->piece, endCoordinate);
}

static void goToEndOfGame(FILE* file, Vector* board, Vector* moves) {
    while (hasNextMove(file)) {
        goOneMoveForward(file, board, moves);
    }
}

static byte executeReplaySelection(byte selection, FILE* file, Vector* pieceStartingPositions, Vector** board, Vector** pieces, Vector** moves) {
    switch (selection) {
        case 1:
            goToStartOfGame(file, pieceStartingPositions, board, pieces, moves);
            break;
        case 2:
            goOneMoveBackward(file, *board, *pieces, *moves);

            break;
        case 3:
            goOneMoveForward(file, *board, *moves);

            break;
        case 4:
            goToEndOfGame(file, *board, *moves);

            break;
        case 5:
            return 0;
        default:
            printf("Invalid selection\n");
            break;
    }

    return 1;
}

void replayGame() {
    clearScreen();

    getchar();

    printf("Which file do you want to replay from?\n> ");

    char fileName[MAX_FILE_NAME_LENGTH];
    fgets(fileName, MAX_FILE_NAME_LENGTH, stdin);

    // remove the \n from the end of the string
    fileName[strlen(fileName) - 1] = '\0';

    byte boardSize, numberOfPieces;
    Vector *pieceStartingPositions, *board = NULL, *pieces = NULL, *moves = NULL;

    FILE* file = fopen(fileName, "rb");
    if (file == NULL) {
        printf("Failed to open file");

        exit(1);
    }

    // read the first 2 bytes
    fread(&boardSize, sizeof(byte), 1, file);
    fread(&numberOfPieces, sizeof(byte), 1, file);

    pieceStartingPositions = initVector();

    for (byte i = 0; i < numberOfPieces; i++) {
        byte position[2];
        fread(position, sizeof(byte), 2, file);

        Coordinate* coordinate = createCoordinate(position[0], position[1]);

        pieceStartingPositions->push(pieceStartingPositions, coordinate);
    }

    initializeReplay(boardSize, pieceStartingPositions, &board, &pieces, &moves);

    runReplayMenu(executeReplaySelection, file, pieceStartingPositions, &board, &pieces, &moves);

    fclose(file);

    // free memory
    freeReplayArgument(pieceStartingPositions);
    freeReplayArguments(board, pieces, moves);
}
