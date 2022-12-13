#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#include "Process_Manager.h"
#include "Command_Manager.h"

/* msleep(): Sleep for the requested number of milliseconds. */
int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0) return -1;
    

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res);

    return res;
}

void setRead(int* lpipe)
{
    dup2(lpipe[0], STDIN_FILENO);
    close(lpipe[0]); // we have a copy already, so close it
    close(lpipe[1]); // not using this end
}

void setWrite(int* rpipe)
{
    dup2(rpipe[1], STDOUT_FILENO);
    close(rpipe[0]); // not using this end
    close(rpipe[1]); // we have a copy already, so close it
}

int forkAndChain(int* lpipe, int* rpipe, char command[])
{
    int pid = fork();
    int status;

    if (pid < 0) {
        return 1;
    }

    if(pid == 0)
    {
        if(lpipe) // there's a pipe from the previous process
            setRead(lpipe);
        // else you may want to redirect input from somewhere else for the start
        if(rpipe) // there's a pipe to the next process
            setWrite(rpipe);
        // else you may want to redirect out to somewhere else for the end
        // sleep(1);
        char **cmd;
        processInput(command, &cmd, " ");
        int status_code = execvp(cmd[0], cmd);
        free(cmd);

        if (-1 == status_code) {
            printf("\n%s: command not found!", command);
            _exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
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
    }
    waitpid(pid,NULL, 0);
    return 0;
}


int executePipes(char **command, int no_pipes) {

    int lpipe[2], rpipe[2];

    // create the first output pipe
    pipe(rpipe);  

    // first child takes input from somewhere else
    forkAndChain(NULL, rpipe, command[0]);  
    // output pipe becomes input for the next process.
    lpipe[0] = rpipe[0];
    lpipe[1] = rpipe[1];

    // chain all but the first and last children
    for(int i = 1; i < no_pipes - 1; i++)
    {
        pipe(rpipe); // make the next output pipe
        forkAndChain(lpipe, rpipe, command[i]);
        close(lpipe[0]); // both ends are attached, close them on parent
        close(lpipe[1]);
        lpipe[0] = rpipe[0]; // output pipe becomes input pipe
        lpipe[1] = rpipe[1];
    }

    // fork the last one, its output goes somewhere else      
    forkAndChain(lpipe, NULL, command[no_pipes]);
    close(lpipe[0]);
    close(lpipe[1]);

    for (int i = 0; i < no_pipes; i++) {
        wait(NULL);
        msleep(50);
    }

    return 0;
}
