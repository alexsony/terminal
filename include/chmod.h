#ifndef CHMOD_H
#define CHMOD_H

#include <sys/stat.h>


int OPTION_EXECUTE;
int OPTION_READ;
int OPTION_WRITE;

static struct stat fs;

int executeChmod(int , char *[]);
void resetChmodOptions();
void readChmodOptions(char *);
void writeChmodOptions(char *);
void setFileRead(char *, struct stat *);
void clearFileRead(char *,struct stat *);
void setFileWrite(char *, struct stat *);
void clearFileWrite(char *, struct stat *);
void setFileExecute(char *, struct stat *);
void clearFileExecute(char *, struct stat *);

#endif  