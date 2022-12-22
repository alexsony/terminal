#ifndef CHMOD_H
#define CHMOD_H

int OPTION_EXECUTE;
int OPTION_READ;
int OPTION_WRITE;

int executeChmod(int , char *[]);
void resetChmodOptions();
void readChmodOptions(char *);
void writeChmodOptions(char *);
void setFileRead(char *);
void clearFileRead(char *);
void setFileWrite(char *);
void clearFileWrite(char *);
void setFileExecute(char *);
void clearFileExecute(char *);

#endif  