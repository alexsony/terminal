#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "Process_Manager.h"
#include "Command_Manager.h"
#include "diff.h"

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

void getData(char *data) {
    memset(data,0,strlen(data));
    strcat(data, getUser());
    strcat(data, ":~");
    strcat(data, getDirectory());
    strcat(data, "$ ");
}

int getch() {
    static struct termios oldtc, newtc;
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
    else executePipes(split_command, no_pipes);

    free(split_command);
}

//VT100 escape codes
int runTerminal() {
    int ch, position = 0, history_index = 0;
    int history_memory = 10, history_search = 0;
    int command_length = 0, up_lines = 1, check_window = 0, count_lines = 0;
    char command[255] = { 0 };
    char details[255];
    char **history = (char**)malloc(sizeof(char *) * history_memory);

    struct winsize w;

    printLogo();
    printf("\r%s:~%s$ ",getUser(),getDirectory());
    do {
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
        ch = getch();
        getData(details);
        switch(ch) {
        case KEY_END:
            break;

        case KEY_UP:
            if (history_index > history_search) {
                history_search++;
                printf("\33[2K\r%s%s",details, history[history_index - history_search]);
                memset(command, 0, command_length);
                strcpy(command,history[history_index - history_search]);
                fflush(stdout);
            }
            continue;
        
        case KEY_DOWN:
            if (1 < history_search) {
                history_search--;
                printf("\33[2K\r%s%s", details, history[history_index - history_search]);
                strcpy(command,history[history_index - history_search]);
                fflush(stdout); 
            }
            continue;
        
        case KEY_BACKSPACE:
            command[position]  = '\0';
            position--;
            printf("\33[2K\r%s%s",details,command);
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
                up_lines = 1;
            }

            memset(command, 0, command_length);
            position = 0; 

            if (history_index > history_memory) {
                history_memory *= 2;
                history = (char**)realloc(history, sizeof(char *) * history_memory);
            }
            fflush(stdout);
            printf("\n\r%s",details);
            continue;

        default:
            command[position] = (char)ch;
            position++;
            check_window = strlen(command) + strlen(details);
            if (check_window > w.ws_col) {
                count_lines++;
                if (count_lines % w.ws_col == 0) {
                    up_lines++; 
                    count_lines = 0;
                }
                printf("\033[%dA[K\r%s%s", up_lines, details,command);
            } else {
                printf("\r%s%s", details,command);

            }
        } 
        
    } while (KEY_END != ch);
    printf("\nBye Bye!\n");

    // for (int i=0; i<history_memory; i++) {
    //     free(history[i]);
    // }
    free(history);
}

int main() {

    // runTerminal();

    int argc;
    char **command;
    char input[] = {"diff -a /home/alex/workplace/custom_terminal/file1.txt /home/alex/workplace/custom_terminal/file2.txt"};
 
    argc = processInput(input, &command, " ");  
   
    executeDiff(argc, command);

    return 0;
}