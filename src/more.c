#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "more.h"

#define MAXLINES 1000

int executeMore(int argc, char *argcv[]) {
    char* file_lines[MAXLINES]; 

    if (argc <1) {
        fprintf(stderr,"Error: Too few arguments!\n");
        exit(EXIT_FAILURE);
    }
    resetMoreOptions();
    for (int i = 1; i <= argc - 1; ++i) {
        checkMoreArgument(argcv[i]);
    }
    printf("NO_DISPLAY_LINES = %d\n", NO_DISPLAY_LINES);
    printf("OPTION_LINES = %d\n", OPTION_LINES);
    printf("OPTION_D = %d\n", OPTION_D);
    printf("OPTION_S = %d\n", OPTION_S);

    int file_length = readMoreFile(file_lines, argcv[argc]);
}

void checkMoreArgument(char *arg) {
    if(arg[0] != '-') {
        fprintf(stderr,"more: Bad argument: \'%s\'\n", arg);
        exit(EXIT_FAILURE);
    }
    if (strcmp(arg, "-d") == 0) OPTION_D = 1;
    else if (strcmp(arg, "-s") == 0) OPTION_S = 1;
    else if(isNumber(arg)) OPTION_LINES = 1;
    else {
        fprintf(stderr,"more: Invalid argument: \'%s\'\n", arg);
        exit(EXIT_FAILURE);
    }
}

void resetMoreOptions() {
    OPTION_LINES = 0;
    NO_DISPLAY_LINES = 0;
    OPTION_D = 0;
    OPTION_S = 0;
}

int readMoreFile(char* arr[], char* name)
{
    FILE *file;
    char line[MAXLINES];
    int index = 0;
    file = fopen(name, "r");
    if (file == NULL) {
        fprintf(stderr,"Can't open File: %s\n", name);
        exit(EXIT_FAILURE);
    }
    while (fgets(line, MAXLINES, file) != NULL) {
        // if (line[strlen(line) - 1] == '\n')
        //     line[strlen(line) - 1] = '\0';  //Removes newline char from end of string
        arr[index] = malloc(sizeof(line));
        strcpy(arr[index], line);
        index++;
    }
    // for(int i = 0; i < index; ++i) {
    //     for(int j = 0; j < strlen(arr[i]);j++)  {
    //         if ((!isascii(arr[i][j]) || iscntrl(arr[i][j])) && !isspace(arr[i][j])) {
    //             IS_BINARY = 1;
    //             break;
    //         }
    //     }
    // }
    fclose(file);
    return index;
}

int isNumber(char s[]) {
    char *number = s + 1;
    for (int i = 0; number[i]!= '\0'; ++i)
    {
        if (isdigit(number[i]) == 0) return 0;
    }
    NO_DISPLAY_LINES = atoi(number);
    return 1;
}