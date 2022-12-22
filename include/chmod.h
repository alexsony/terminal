#ifndef CHMOD_H
#define CHMOD_H

int OPTION_EXECUTE;
int OPTION_READ;
int OPTION_WRITE;

int executeChmod(int , char *[]);
static void resetOptions();
void readOptions(char *);

#endif