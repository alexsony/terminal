#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "Command_Manager.h"
#include "diff.h"
#include "chmod.h"

int processInput(char command[], char *** split, const char *delim) {
    char *tmp_str = command;
    int count = 0; 

    //Count elements
    while (*tmp_str)
    {
        if (delim[0] == *tmp_str) count++;
        tmp_str++; 
    }

    count++; //for NULL
    *split = (char **)malloc(sizeof(char*) * count);
    char *token = strtok(command,delim);

    count = 0;
    while(token != NULL) {
        (*split)[count] = (char*) malloc(sizeof(char) * strlen(token));
        (*split)[count] = token;
        token = strtok(NULL,delim);
        count++;
    }
    (*split)[count] = NULL;

    return --count ;
}

int handleCustomCommand(const char *command, char * argv[], int no_argc) {
    int no_of_command = 4, search_command = 0, status = 1;
    char * list_of_commands[no_of_command];

    list_of_commands[0] = "help";
    list_of_commands[1] = "version";
    list_of_commands[2] = "diff";
    list_of_commands[3] = "chmod";

    for (int i = 0; i < no_of_command; ++i) {
        if (strcmp(command, list_of_commands[i]) == 0) {
            search_command = i + 1;
            break;
        }
    }

    switch (search_command)
    {
    case 1:
        printf("You can use the following commands:\n");
        for (int i = 0; i < no_of_command; ++i) {
            printf("%d: %s\n",i + 1, list_of_commands[i]);
        }
        status = 0;
        break;
    case 2:
        printf("Version 1.00 by Alexandru Isvoranu\n");
        status = 0;
        break;
    case 3:
        executeDiff(no_argc, argv);
        status = 0;
        break;
    case 4:
        executeChmod(no_argc, argv);
        status = 0;
        break;
    default:
        break;
    }

    return status;
}