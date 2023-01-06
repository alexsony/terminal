#ifndef MORE_H
#define MORE_H

int NO_DISPLAY_LINES;
int OPTION_LINES;
int OPTION_D;
int OPTION_S;
int IS_PIPED;

int executeMore(int , char *[], int);
int readMoreFile(char* [], char*);
void checkMoreArgument(char *);
void resetMoreOptions();
void displayMore(int, char * []);
void runLines(int, char * []);
void printMoreDetails(int, int);
int isNumber(char []);

#endif