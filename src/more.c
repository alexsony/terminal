#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "more.h"
#include "Terminal_Manager.h"

#define MAXLINES 1000

int executeMore(int argc, char *argcv[]) {
    char* file_lines[MAXLINES]; 

    if (argc <1) {
        fprintf(stderr,BRED"Error: "RESET"Too few arguments!\n");
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
    displayMore(file_length, file_lines);
    exit(EXIT_SUCCESS);
}

void checkMoreArgument(char *arg) {
    if(arg[0] != '-') {
        fprintf(stderr,"more: "BRED"Bad argument:"RESET"\'%s\'\n", arg);
        exit(EXIT_FAILURE);
    }
    if (strcmp(arg, "-d") == 0) OPTION_D = 1;
    else if (strcmp(arg, "-s") == 0) OPTION_S = 1;
    else if(isNumber(arg)) OPTION_LINES = 1;
    else {
        fprintf(stderr,"more: " BRED"Invalid argument:"RESET"\'%s\'\n", arg);
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
    int index = 0, is_blank_line = 0;
    file = fopen(name, "r");
    if (file == NULL) {
        fprintf(stderr, BRED "Error: " RESET "Can't open File: %s\n", name);
        exit(EXIT_FAILURE);
    }
    while (fgets(line, MAXLINES, file) != NULL) {
        if (line[0] == '\n') is_blank_line++;
        else is_blank_line = 0;

        if ((is_blank_line < 2) && (OPTION_S)) {
            arr[index] = malloc(sizeof(line));
            strcpy(arr[index], line);
            index++;
        }

        if(OPTION_S == 0) {
            arr[index] = malloc(sizeof(line));
            strcpy(arr[index], line);
            index++;
        }
    }

    fclose(file);
    return index;
}

void displayMore(int length, char *file[]) {
    if (OPTION_LINES == 0) {
        for(int i = 0; i < length; ++i) printf("%s",file[i]);
    } else if ((OPTION_LINES) && (NO_DISPLAY_LINES > 0)) {
        runLines(length, file);
    }
}

void runLines(int length, char *file[]) {
    int i, ch;
    for (i = 0; i <  NO_DISPLAY_LINES; ++i) 
        printf("%s", file[i]);

    printMoreDetails(i, length);

    do {
        ch = getch();

        if (KEY_ENTER == ch) {
            printf("\33[2K\r%s", file[i]);
            i++;
            printMoreDetails(i, length);
        }

        if (KEY_SPACEBAR == ch) {
            for (int j = 0; j < 5; ++j) {
                printf("\33[2K\r%s", file[i]);
                i++;
            }
            printMoreDetails(i, length);
        }

        if ('q' == (char)ch) break;
    } while (i < length);
    printf("\33[2K\r");
}

void printMoreDetails(int current_index, int length) {
    if (OPTION_D) {
        printf(BBLK WHTB "--More--(%d/%d lines)[Press space to continue, 'q' to quit.]" RESET,current_index,length);
    } else {
        printf(BBLK WHTB "--More--(%d/%d lines)" RESET,current_index,length);
    }
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