#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <termios.h> 

#include "vector.h"

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
   ch=getchar();//read char
   //ch=getchar();
   //ch=getchar();
   //enable echo flag
   tcsetattr(STDIN_FILENO, TCSANOW, &oldtc);
   return ch;
}

//VT100 escape codes
int searchHistory() {
    int ch, position = 0;
    char command[255];
    char arrow_command[] = "Arrow";
    while(1) {
        ch = getch();
        ///printf("%d\n", ch);
        if(ch == 56) {
            printf("\33[2K\rUP arrow");
            fflush(stdout);
            continue;
        }
        else if(ch == 50) {
            printf("\33[2K\rdown arrow");
            fflush(stdout); 
            continue;
        } else if(ch == 27) {
            break; 
        } else if(ch == 10) {
            position = 0; 
            memset(command, 0, 255);
            printf("\n");
        } else {
            command[position] = (char)ch;
            position++;
            printf("\33[2K\r%s",command);
            fflush(stdout); 
        } 
    }
    printf("Outside while");
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

int main() {

    char s1[] = "ping -c 5 google.com";
    char s2[] = "grep rtt";
    //executePipes(s1,s2);
    //processCommand(s1);

    vector v;
    vectorInit(&v);
    vectorAdd(&v,s1);
    vectorAdd(&v,s2);

    // for (int i = 0; i < vectorTotal(&v); i++)
    //     printf("%s\n", (char *) vectorGet(&v, i));
    // printf("\n");

    int ch;
    char command[100];
    searchHistory();
            // if(searchHistory() == 0) {
            //     scanf("%s",command);
            //     printf("The Entered string");
            // }
            //break;
        //     printf("wrong input \n");

        // break;
        
    
    return 0;
}