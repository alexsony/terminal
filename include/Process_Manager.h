#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

int processInput(char [], char *** , const char *);
int handleCustomCommand(const char *, char *const []);
int executeCommand(char []);
int executePipes(char [], char []);
int multiPipes(char **, int );

#endif