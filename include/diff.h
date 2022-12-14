#ifndef DIFF_H
#define DIFF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int executeDiff(char *, char *);
int compareFiles(FILE *, FILE *);
int storeContent(char ***, FILE *);
void processEqualCase(char **, char **, int );


#endif