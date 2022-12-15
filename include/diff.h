#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINES 1000
#define MAXLENGTH 256

typedef struct instructions {
    int operation;
    int left;
    int right;
    char line[MAXLENGTH];
} instructions;

typedef struct LineDetails {
    int operation;
    int leftStartPos;
    int leftEndPos;
    int rightStartPos;
    int rightEndPos;
    char line[MAXLENGTH];
} LineDetails;

int OPTION_BRIEF;
int OPTION_FORCECOMPARE;

int instructionInfoLength;
instructions instructionInfo[MAXLINES * 2];

int executeDiff(int , char *[]);
LineDetails DiffLines(int, int, int, int, int);
void outputDiff(LineDetails [], int, char* [], char* []);
void resetOptions();
void addInstruction(int, int, int, char*);
int readFile(char* [], char*, int);
void freeMemory(char* [], int);
void insertInTable(int [MAXLINES][MAXLINES], char* [], char* [], int, int);
void diff(int [MAXLINES][MAXLINES], char* [], char* [], int, int);
int formatLines(LineDetails []);

#endif