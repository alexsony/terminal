#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

void setRead(int* );
void setWrite(int* );
int forkAndChain(int* , int* , char []);
int forkAndRedirect(int*, char [], char[]);
int executeCommand(char []);
int executePipes(char **, int, char [], int); 

#endif