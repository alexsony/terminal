#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "diff.h"

int min(int a, int b, int c) {
  return a < b ? (a < c ? a : c): (b < c ? b : c);
}

void freeMemory(char* arr[], int lineCount)
{
    for (int i = 0; i < lineCount; i++) {
        if (arr[i] == NULL) return;
        free(arr[i]);}
}

static void resetOptions(){
    OPTION_BRIEF = 0;
    OPTION_FORCECOMPARE = 0;
    IS_BINARY = 0;
}

int readFile(char* arr[], char* name)
{
    FILE *file;
    char line[MAXLENGTH];
    int index = 0;
    file = fopen(name, "r");
    if (file == NULL) {
        fprintf(stderr,"Can't open File: %s\n", name);
        exit(EXIT_FAILURE);
    }
    while (fgets(line, MAXLENGTH, file) != NULL) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';  //Removes newline char from end of string
        arr[index] = malloc(sizeof(line));
        strcpy(arr[index], line);
        index++;
    }
    for(int i = 0; i < index; ++i) {
        for(int j = 0; j < strlen(arr[i]);j++)  {
            if ((!isascii(arr[i][j]) || iscntrl(arr[i][j])) && !isspace(arr[i][j])) {
                IS_BINARY = 1;
                break;
            }
        }
    }
    fclose(file);
    return index;
}

void printDiff(int d_table[MAXLINES][MAXLINES], char* file_1[], char* file_2[], int f_length_1, int f_length_2) {

    int max = f_length_1 > f_length_2 ? f_length_1 : f_length_2;
    char *output[max];
    int output_index = 0;
    int is_equal = 1;
    while ((f_length_1 > 0) && (f_length_2 > 0)) {
        //lines are the same
        if (strcmp(file_1[f_length_1 - 1], file_2[f_length_2 - 1]) == 0) {
            f_length_1--;
            f_length_2--;
        //lines need to be changed
        } else if (d_table[f_length_1][f_length_2] == d_table[f_length_1 - 1][f_length_2 - 1] + 1) {
            asprintf(&output[output_index],"%dc%d\n< %s\n> %s\n---\n",  f_length_1, 
                                                f_length_2,
                                                file_1[f_length_1 - 1],
                                                file_2[f_length_2 - 1]);
            f_length_1--;
            f_length_2--;
            output_index++;
            is_equal = 0;
        //line need to be deleted
        } else if (d_table[f_length_1][f_length_2] == d_table[f_length_1 - 1][f_length_2] + 1) {
            asprintf(&output[output_index],"%dd\n< %s\n---\n", f_length_1 - 1, 
                                       file_1[f_length_1 - 1]);
            f_length_1--;
            output_index++;
            is_equal = 0;
        //Add line
        } else if (d_table[f_length_1][f_length_2] == d_table[f_length_1][f_length_2 - 1] + 1) {
            asprintf(&output[output_index], "%da%d\n> %s\n---\n",    f_length_1,
                                            f_length_2,             
                                            file_2[f_length_2- 1]);
            f_length_2--;
            output_index++;
            is_equal = 0;
        }
    }
    for (int i = f_length_1 - 1; i >= 0; i--) {
        asprintf(&output[output_index],"%dd\n< %s\n---\n",  i, 
                                    file_1[i]);
        output_index++;
        is_equal = 0;
    }
    if(OPTION_BRIEF) {
        is_equal ? printf("Files are the same\n") : printf("Files are different\n");
    } else if((1 == IS_BINARY) && (0 == OPTION_FORCECOMPARE)){
        is_equal ? printf("Binary files are the same\n") : printf("Binary files are different\n");
    }  else {
        for(int i = 0; i < output_index; i++) printf("%s", output[i]);
    }
}

void getEditDistance(char* file_1[], char* file_2[], int f_length_1, int f_length_2) {
    //table to store results
    int d_table[MAXLINES][MAXLINES];

    for (int i = 0; i <= f_length_1; i++) d_table[i][0] = i;
    for (int j = 0; j <= f_length_2; j++) d_table[0][j] = j;

    //fill the table
    for (int i = 1; i <= f_length_1; i++) {
        for (int j = 1; j <= f_length_2; j++) {

            //if lines are the same, ignore them and continue with next one
            if (strcmp(file_1[i - 1], file_2[j - 1]) == 0)
                d_table[i][j] = d_table[i - 1][j - 1];
            else 
                d_table[i][j] = min(d_table[i - 1][j - 1],  //Change
                                    d_table[i - 1][j],      //Delete
                                    d_table[i][j - 1]) + 1; //Add
        }
    }
    printDiff(d_table, file_1, file_2, f_length_1, f_length_2);
}

int executeDiff(int argc, char *argv[]) {
char option = 0;
char file1[MAXLENGTH], file2[MAXLENGTH];   // File Buffers
char* file_lines_1[MAXLINES];      // String arrays holding strings pertaining to their files
char* file_lines_2[MAXLINES];

resetOptions();
    
if (argc == 1) {
    fprintf(stderr,"Error: Not Enough Arguments\n");
    exit(EXIT_FAILURE);
}
for (int i = 1; i <= argc - 2; i++) {
    char flag[MAXLENGTH]; 
    strcpy(flag , argv[i]); //Copies argument to flag for later comparision

    if (strcmp(flag, "-q") == 0)        OPTION_BRIEF = 1;
    else if (strcmp(flag, "-a") == 0)   OPTION_FORCECOMPARE = 1;
    else {
        fprintf(stderr,"Invalid option: %s\n", flag);
        exit(EXIT_FAILURE);
    }
}
    strcpy(file1, argv[argc - 1]);
    strcpy(file2, argv[argc]);

    int file_length_1 = readFile(file_lines_1, file1);
    int file_length_2 = readFile(file_lines_2, file2);
    getEditDistance(file_lines_1, file_lines_2, file_length_1, file_length_2);

    freeMemory(file_lines_1, file_length_1);
    freeMemory(file_lines_2, file_length_2);

    exit(EXIT_SUCCESS);

}
