#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

void set_read(int* );
void set_write(int* );
int fork_and_chain(int* , int* , char []);
int executeCommand(char []);
int executePipes(char **, int ); 

#endif