#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINES 1000
#define MAXLENGTH 256


int OPTION_BRIEF;
int OPTION_FORCECOMPARE;
int IS_BINARY;


int executeDiff(int , char *[]);
void getEditDistance(char* [], char* [], int, int);
int readFile(char* [], char*);
void freeMemory(char* [], int);
void resetOptions();
void printDiff(int [MAXLINES][MAXLINES], char* [], char* [], int, int);


#endif