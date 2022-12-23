#ifndef MORE_H
#define MORE_H

int NO_DISPLAY_LINES;
int OPTION_LINES;
int OPTION_D;
int OPTION_S;

int executeMore(int , char *[]);
int readMoreFile(char* [], char*);
void checkMoreArgument(char *);
void resetMoreOptions();
int isNumber(char []);


#endif