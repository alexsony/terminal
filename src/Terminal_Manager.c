#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <termios.h>

#include "Terminal_Manager.h"

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