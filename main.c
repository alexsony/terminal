#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h>
// #include <ncurses.h>

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
    //ch=getchar();
    //ch=getchar();
    //enable echo flag
    tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
    return ch;
}

int processInput(char command[], char *** split, const char *delim) {
    printf("%s", command);
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
        // printf("Token: %s \n",token);
        (*split)[count] = token;
        // printf("Token: %s \n",split[count]);
        token = strtok(NULL,delim);
        count++;
    }
    // printf("Token: %s \n",split[1]);
    (*split)[count] = NULL;

    return --count ;
}

int executeCommand(char command[]) {
    int pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        char **cmd; 
        processInput(command, &cmd, " "); 
        int status_code = execvp(cmd[0], cmd);
        free(cmd);
        if (-1 == status_code) {
            printf("\n%s: command not found!", command);
            exit(1);
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

void runCommand(char command[]) {
    char **split_command;
    int no_pipes = processInput(command, &split_command, "|");

    if (0 == no_pipes) executeCommand(split_command[no_pipes]);
    else if(1 == no_pipes) executePipes(split_command[0], split_command[1]);
    // else multiPipes(split_command, no_pipes);

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
    printf("\r%s:%s$",getUser(),getDirectory());
    do {
        ch = getch();
        switch(ch) {
        case KEY_END:
            break;

        case KEY_UP:
            if (history_index > history_search) history_search++;
            printf("\33[2K\rUP: %s", history[history_index - history_search]);
            strcpy(command,history[history_index - history_search]);
            fflush(stdout);
            continue;
        
        case KEY_DOWN:
            if (1 < history_search) history_search--;
            printf("\33[2K\rDOWN: %s", history[history_index - history_search]);
            strcpy(command,history[history_index - history_search]);
            fflush(stdout); 
            continue;
        
        case KEY_BACKSPACE:
            command[position]  = '\0';
            position--;
            printf("\33[2K\r%s:%s$%s", getUser(),getDirectory(),command);
            continue;

        case KEY_ENTER:
            command_length = strlen(command); 
            if(command_length) {
                history[history_index] = (char *)malloc(sizeof(char) * command_length);
                strcpy(history[history_index], command);
                runCommand(command);
            }

            memset(command, 0, command_length);
            // printf("\nHistory : %s\n", history[history_index]);
            printf("\n");
            position = 0; 
            history_index++;
            history_search = 0;

            if (history_index > history_memory) {
                history_memory *= 2;
                history = realloc(history, sizeof(char *) * history_memory);
            }
            printf("\33[2K\r%s:%s$",getUser(),getDirectory());
            continue;

        default:
            command[position] = (char)ch;
            position++;
            printf("\33[2K\r%s:%s$%s", getUser(),getDirectory(),command);
            fflush(stdout); 
        } 
        
    } while (KEY_END != ch);
    printf("\nBye Bye!\n");

    for (int i=0; i<history_index; i++) {
        free(history[i]);
    }
    free(history);
}

int main() {

    char command1[] = "ps aux";
    char command2[] = "grep root";
    char command3[] = "grep sbin";
    char **split_command;

    char command[] = "ps aux | grep root | grep sbin | gredqp usr";
    int pipes = processInput(command, &split_command, "|");

    runTerminal();
    // multiPipes(split_command, pipes);
    // printf("%s", command[0]);
    // test1(command1, command2, command3);

    // printLogo();
    return 0;
}