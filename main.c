#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>

#include "Process_Manager.h"

#define KEY_ENTER 10
#define KEY_END 70
#define KEY_UP 65
#define KEY_DOWN 66
#define KEY_RIGHT 67
#define KEY_LEFT 68
#define KEY_BACKSPACE 127
#define DELIMITER 27


int printLogo() {

    printf("\033[H\033[J");
    //read any text file from currect directory
    char const* const fileName = "/home/alex/workplace/custom_terminal/logo.txt";

    FILE* file = fopen(fileName, "r"); 

    if(!file){
    printf("\n Unable to open : %s ", fileName);
    return -1;
    }

    char line[500];

    while (fgets(line, sizeof(line), file)) {
    printf("%s", line); 
    }
    fclose(file);
    return 0;
}

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

int getch() {
    struct termios oldtc;
    struct termios newtc;
    int ch;
    //get the parameters asociated with the terminal 
    tcgetattr(STDIN_FILENO, &oldtc);
    newtc = oldtc;
    //c_lflag = control terminal functions
    //here disable echo for input
    /*ICANON normally takes care that one line at a time will be processed
    that means it will return if it sees a "\n" or an EOF or an EOL*/
    newtc.c_lflag &= ~(ICANON | ECHO);
    //set the parameters asociated with the terminal 
    //TCSANOW the changes will occur immediately
    tcsetattr(STDIN_FILENO, TCSANOW, &newtc);
    ch = getchar();//read char
    if (DELIMITER == ch) {
        ch = getchar();
        ch = getchar();
    }
    //enable echo flag
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    return ch;
}


void runCommand(char command[]) {
    char **split_command;
    int no_pipes = processInput(command, &split_command, "|");

    if (0 == no_pipes) executeCommand(command);
    else if(1 == no_pipes) executePipes(split_command[0], split_command[1]);
    else multiPipes(split_command, no_pipes);

    free(split_command);
}

//VT100 escape codes
int runTerminal() {
    int ch, position = 0, history_index = 0;
    int history_memory = 10, history_search = 0;
    int command_length = 0;
    char command[255] = { 0 };
    char **history = (char**)malloc(sizeof(char *) * history_memory);

    printLogo();
    printf("\r%s:%s$ ",getUser(),getDirectory());
    do {
        ch = getch();
        switch(ch) {
        case KEY_END:
            break;

        case KEY_UP:
            if (history_index > history_search) {
                history_search++;
                printf("\33[2K\rUP: %s", history[history_index - history_search]);
                memset(command, 0, command_length);
                strcpy(command,history[history_index - history_search]);
                fflush(stdout);
            }
            continue;
        
        case KEY_DOWN:
            if (1 < history_search) {
                history_search--;
                printf("\33[2K\rDOWN: %s", history[history_index - history_search]);
                strcpy(command,history[history_index - history_search]);
                fflush(stdout); 
            }
            continue;
        
        case KEY_BACKSPACE:
            command[position]  = '\0';
            position--;
            printf("\33[2K\r%s:%s$ %s", getUser(),getDirectory(),command);
            fflush(stdout); 
            continue;

        case KEY_ENTER:
            command_length = strlen(command); 
            if(command_length) {
                history[history_index] = (char *)malloc(sizeof(char) * command_length);
                strcpy(history[history_index], command);
                printf("\n");
                runCommand(command);

                history_index++;
                history_search = 0;
            }

            memset(command, 0, command_length);
            position = 0; 

            if (history_index > history_memory) {
                history_memory *= 2;
                history = realloc(history, sizeof(char *) * history_memory);
            }
            printf("\n\33[2K\r%s:%s$ ",getUser(),getDirectory());
            continue;

        default:
            command[position] = (char)ch;
            position++;
            printf("\33[2K\r%s:%s$ %s", getUser(),getDirectory(),command);
            fflush(stdout); 
        } 
        
    } while (KEY_END != ch);
    printf("\nBye Bye!\n");

    // for (int i=0; i<history_index; i++) {
    //     free(history[i]);
    // }
    free(history);
}

int main() {

    runTerminal();
    return 0;
}