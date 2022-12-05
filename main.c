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
#define DELIMITER 27


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

    return args;
    
}

int executeCommand(char command[]) {
    int pid = fork();
    if (pid < 0) {
        return 1;
    }

    if (pid == 0) {
        char **cmd = processCommand(command); 
        int status_code = execvp(cmd[0], cmd);
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
        char **left = processCommand(left_command);
        execvp(left[0], left);
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
        execlp("cat", "cat", NULL);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;

}

//VT100 escape codes
int runTerminal() {
    int ch, position = 0, history_index = 0;
    int history_memory = 10, history_search = 0;
    int command_length = 0;
    char command[255] = { 0 };
    char **history = (char**)malloc(sizeof(char *) * history_memory);

    printf("%s:%s$",getUser(),getDirectory());
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

        case KEY_ENTER:
            //printf("\33\r\nCommand: %s\n", command); 
            executeCommand(command);

            command_length = strlen(command); 
            history[history_index] = (char *)malloc(sizeof(char) * command_length);

            strcpy(history[history_index], command);
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

    char s1[] = "ping -c 5 google.com";
    char s2[] = "grep rtt";
    //executePipes(s1,s2);
    //processCommand(s1);

    int ch;
    char command[100];
    runTerminal();
    // execlp("ping", "ping", "-c", "1","google.com",NULL);
    // executeCommand(s1);
    // test();
    return 0;
}