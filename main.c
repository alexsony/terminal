#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ncurses.h>

#include "vector.h"

#define KEY_ENTER_MAIN 10

char* getDirectory() {
    static char cwd[255];
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

char *getUser() {
    static char username[255];
    getlogin_r(username,255);
    return username;
}

char** processCommand(char command[]) {
    char **args = 0;
    const char *delim = " ";
    char *tmp_str = command;
    int count = 0; 

    //Count elements
    while (*tmp_str)
    {
        if (delim[0] == *tmp_str) count++;
        tmp_str++; 
    }

    count++; //for NULL
    args = malloc(sizeof(char*) * count);
    char *token = strtok(command,delim);

    count = 0;
    while(token != NULL) {
        //printf("Token: %s \n",token);
        args[count] = token;
        token = strtok(NULL,delim);
        count++;
    }
    args[count] = NULL;

    //execvp(args[0], args);
    return args;
    
}

//file descriptor is unique number acrros process  =>handle of the input/output resource
int executePipes(char left_command[], char right_command[]) {
    int fd[2]; //file descriptor fd[0] -> read end of pipe; fd[1] -> write end of pipe
    if (pipe(fd) == -1) {
        return 1;
    }

    int pid1= fork();
    if (pid1 < 0) {
        return 2;
    }

    if (pid1 == 0) {
        //Child process 1
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        close(fd[1]);
        char **left = processCommand(left_command);
        execvp(left[0], left);
    }

    int pid2 = fork();
    if (pid2 < 0) {
        return 3;
    }

    if (pid2 == 0) {
        //Child process 2
        dup2(fd[0], STDIN_FILENO);
        close(fd[0]);
        close(fd[1]);
        execlp("grep", "grep", "rtt", NULL);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;

}


int runTerminal() {
    int ch, position = 0, history_index = 0;
    int y, x, history_memory = 10; 
    char command[255] = { 0 };
    char **history = (char**)malloc(sizeof(char *) * history_memory);
    
    initscr();
    raw();
    keypad(stdscr, TRUE);
    printw("Welcome to Terminal\n");
    printw("%s:%s$",getUser(),getDirectory());

    do {
        noecho();

        ch = getch();
        getyx(stdscr, y, x);
        switch(ch) {
        case KEY_UP:
            move(y, 0);      
            clrtoeol();  
            printw("Up Key");
            break;

        case KEY_DOWN:
            move(y, 0);      
            clrtoeol();  
            printw("Down Key");
            break;

        case KEY_ENTER_MAIN:
            printw("\nCommand: %s", command);
            history[history_index] = (char *)malloc(sizeof(char) * position);

            ///strcpy(history[history_index], command);
            memset(command, 0, position);
            //printw("\nhistory here: %s\n", history[history_index]);
            position = 0; 
            history_index++;

            if (history_index > history_memory) {
                history_memory *= 2;
                history = realloc(history, sizeof(char *) * history_memory);
            }
            printw("%s:%s$",getUser(),getDirectory());

            break;

        default:
            command[position] = (char)ch;
            position++;
            // move(y, strlen(getDirectory()));
            move(y, 0);
            printw("%s:%s$%s", getUser(),getDirectory(),command);
            //printw("%s",command);

        }

    } while(KEY_END != ch);
    endwin();

    for (int i=0; i<history_index; i++) {
        free(history[i]);
    }
    free(history);

}

int main() {

    char s1[] = "ping -c 5 google.com";
    char s2[] = "grep rtt";
    //executePipes(s1,s2);
    //processCommand(s1);

    int ch;
    char command[100];
    runTerminal();
    // execlp("ping", "ping", "-c", "1","google.com",NULL);
    
    return 0;
}