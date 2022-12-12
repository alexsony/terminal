#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "Process_Manager.h"

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

int handleCustomCommand(const char *command, char *const argv[]) {
    int no_of_command = 2, search_command = 0, status = 1;
    char * list_of_commands[no_of_command];

    list_of_commands[0] = "help";
    list_of_commands[1] = "version";

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
            printf("%d: %s\n",i, list_of_commands[i]);
        }
        status = 0;
        break;
    case 2:
        printf("Version 1.00 by Alexandru Isvoranu\n");
        status = 0;
        break;
    default:
        break;
    }

    return status;
}

int executeCommand(char command[]) {
    int pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        char **cmd; 
        processInput(command, &cmd, " "); 
        int check_type_command = handleCustomCommand(cmd[0], cmd);
        if (check_type_command) {
            int status_code = execvp(cmd[0], cmd);
            free(cmd);
            if (-1 == status_code) {
                printf("\n%s: command not found!", command);
                exit(1);
            }
        } else {
            exit(EXIT_SUCCESS);
        }
    } else {
        wait(NULL);

    }

    return 0;
}

//file descriptor is unique number acrros process  =>handle of the input/output resource
int executePipes(char left_command[], char right_command[]) {
    int fd[2]; //file descriptor fd[0] -> read end of pipe; fd[1] -> write end of pipe
    int status_code;
    if (pipe(fd) == -1) {
        return 1;
    }

    int pid1= fork();
    if (pid1 < 0) {
        return 1;
    }

    if (pid1 == 0) {
        //Child process 1
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);

        char **left;
        processInput(left_command, &left, " ");
        status_code = execvp(left[0], left);
        free(left);

        if (-1 == status_code) {
            printf("\n%s: command not found!", left_command);
            exit(1);
        }
    }

    int pid2 = fork();
    if (pid2 < 0) {
        return 1;
    }

    if (pid2 == 0) {
        //Child process 2
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);

        char **right;
        processInput(right_command, &right, " ");
        status_code = execvp(right[0], right);
        free(right);

        if (-1 == status_code) {
            printf("\n%s: command not found!", right_command);
            exit(1);
        }
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;

}


int multiPipes(char **command, int no_pipes) {
    
    int **fd = (int **)malloc(no_pipes * sizeof(int*));
    for(int i = 0; i < no_pipes; i++) fd[i] = (int *)malloc(2 * sizeof(int));
    int pid, status_code;

    for (int i = 0; i < no_pipes; ++i) {
        if (pipe(fd[i]) == -1) {
            exit(1);
        }
    }

    if ((pid = fork()) == -1) {
        exit(1);
    } else if (pid == 0) {
        dup2(fd[0][1], STDOUT_FILENO);

        close(fd[0][0]);
        close(fd[0][1]);

        char **one;
        processInput(command[0], &one, " ");
        status_code = execvp(one[0], one);

        free(one);

        if (-1 == status_code) {
            printf("\n%s: command not found!", command[0]);
            exit(1);
        }
    }

    for (int i = 0; i < no_pipes - 1; ++i) {
        if ((pid = fork()) == -1) {
            exit(1);
        } else if (pid == 0) {
            dup2(fd[i][0], STDIN_FILENO);
            dup2(fd[i + 1][1], STDOUT_FILENO);

            close(fd[i][0]);
            close(fd[i][1]);
            close(fd[i + 1][0]);
            close(fd[i + 1][1]);

            char **cmd;
            processInput(command[i + 1], &cmd, " ");
            status_code = execvp(cmd[0], cmd);

            free(cmd);

            if (-1 == status_code) {
                printf("\n%s: command not found!", command[i + 1]);
                exit(1);
            }
        }
        close(fd[i][0]);
        close(fd[i][1]);
    }

    if ((pid = fork()) == -1) {
        exit(1);
    } else if (pid == 0) {
        dup2(fd[no_pipes - 1][0], STDIN_FILENO);

        close(fd[no_pipes - 1][0]);
        close(fd[no_pipes - 1][1]);

        char **three;
        processInput(command[no_pipes], &three, " ");
        status_code = execvp(three[0], three);

        free(three);

        if (-1 == status_code) {
            printf("\n%s: command not found!", command[no_pipes]);
            exit(1);
        }
    }

    return 0;

}