#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "diff.h"

#define max(a, b) (a > b ? a : b)

int executeDiff(int argc, char *argv[]) {
char option = 0;
char file1[MAXLENGTH], file2[MAXLENGTH];   // File Buffers
char* file_lines_1[MAXLINES];      // String arrays holding strings pertaining to their files
char* file_lines_2[MAXLINES];
int file_count_1, file_count_2;

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

    int file_length_1 = readFile(file_lines_1, file1, file_count_1);
    int file_length_2 = readFile(file_lines_2, file2, file_count_2);

    int table[MAXLINES][MAXLINES];
    insertInTable(table, file_lines_1, file_lines_2, file_length_1, file_length_2);

    diff(table, file_lines_1, file_lines_2, file_length_1, file_length_2);
    LineDetails lines_info[MAXLINES * 2];
    int lines_info_length = formatLines(lines_info);
    outputDiff(lines_info, lines_info_length, file_lines_1, file_lines_2);
    freeMemory(file_lines_1, file_count_1);
    freeMemory(file_lines_2, file_count_2);

}
void resetOptions(){
    OPTION_BRIEF = 0;
    OPTION_FORCECOMPARE = 0;
}

int readFile(char* arr[], char* name, int lineCount)
{
    FILE *file;
    char line[MAXLENGTH];
    int i = 0;
    lineCount = 0;
    file = fopen(name, "r");
    if (file == NULL) {
        fprintf(stderr,"Can't Open File: %s\n", name);
        exit(EXIT_FAILURE);
    }
    while (fgets(line, MAXLENGTH, file) != NULL) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';  //Removes newline char from end of string
        arr[i] = malloc(sizeof(line));
        lineCount++;
        strcpy(arr[i], line);
        i++;
        }
    fclose(file);
    return i;
}

void insertInTable(int table[MAXLINES][MAXLINES], char* arr1[], char* arr2[], int file_length_1, int file_length_2)
{
    for (int i = 0; i <= file_length_1; i++)  table[0][i] = 0;
    for (int i = 0; i <= file_length_2; i++)  table[i][0] = 0;
    for (int i = 1; i <= file_length_2; i++) {
        for (int j = 1; j <= file_length_1; j++) {
            if (strcmp(arr1[j - 1], arr2[i - 1]) == 0)  table[i][j] = table[i - 1][j - 1] + 1;
            else    table[i][j] = max(table[i - 1][j], table[i][j - 1]);}}
}

void diff(int table[MAXLINES][MAXLINES], char* arr1[], char* arr2[], int col, int row)
{ 
    int differentBool = 0;
    int sameBool = 1;
    while(row > 0 || col > 0) {
        if (row > 0 && col > 0 && strcmp(arr1[col - 1], arr2[row - 1]) == 0) {
            col--;
            row--;}
        else if (col > 0 && row == 0) {
            addInstruction('a', row, col, arr1[col - 1]);
            col--;
            differentBool = 1;
            sameBool = 0;}
        else if (col > 0 && table[row][col - 1] > table[row - 1][col]) {
            addInstruction('a', row, col, arr1[col - 1]);
            col--;
            differentBool = 1;
            sameBool = 0;}
        else if (col > 0 && table[row][col - 1] == table[row - 1][col]) {
            addInstruction('c', row, col, arr1[col - 1]);
            col--;
            row--;
            differentBool = 1;
            sameBool = 0;}
        else if (row > 0 && col == 0) {
            addInstruction('d', row, col + 1, arr2[row - 1]);
            row--;
            differentBool = 1;
            sameBool = 0;}
        else if (row > 0 && table[row][col - 1] < table[row - 1][col]) {
            addInstruction('d', row, col + 1, arr2[row - 1]);
            row--;
            differentBool = 1;
            sameBool = 0;}
    }
    if (OPTION_BRIEF) {
        if (differentBool) printf("The files are different\n");
        else printf("The files are the same\n");
        exit(EXIT_SUCCESS);
    }
    instructions tempArr[MAXLINES * 2];
    for (int i = 0; i < instructionInfoLength; i++) tempArr[i] = instructionInfo[i];
    int arr_counter = 0;
    for (int i = instructionInfoLength - 1; i >= 0; i--) instructionInfo[arr_counter++] = tempArr[i];
}

void addInstruction(int op, int left, int right, char* line) 
{
    instructions newInstruction;
    strcpy(newInstruction.line, line);
    newInstruction.left = left;
    newInstruction.right = right;
    newInstruction.operation = op;
    instructionInfo[instructionInfoLength++] = newInstruction;
}

LineDetails DiffLines(int op, int ls, int le, int rs, int re) {
    LineDetails newDetails;
    newDetails.operation = op;
    newDetails.leftStartPos = ls;
    newDetails.leftEndPos = le;
    newDetails.rightStartPos = rs;
    newDetails.rightEndPos = re;
    return newDetails;
}

int formatLines(LineDetails lines_arr[])
{
    int lastOperation = 0, leftStartPosition = 0, leftEndPosition = 0, rightStartPosition = 0, rightEndPosition = 0, length = 0;
    for (int i = 0; i < instructionInfoLength; i++) {
        instructions tempInstructions = instructionInfo[i];
        if (lastOperation == 0 || lastOperation != tempInstructions.operation || (leftEndPosition + 1 != tempInstructions.left && tempInstructions.operation != 'a') || (rightEndPosition + 1 != tempInstructions.right && tempInstructions.operation != 'd')) {
            if (lastOperation != 0) {
                if (leftEndPosition - leftStartPosition > 0 ) {;}
                else leftEndPosition = -1;
                if (rightEndPosition - rightStartPosition > 0) lines_arr[length++] = DiffLines(lastOperation, leftStartPosition, leftEndPosition, rightStartPosition, rightEndPosition);
                else{
                    rightEndPosition = -1;
                    lines_arr[length++] = DiffLines(lastOperation, leftStartPosition, leftEndPosition, rightStartPosition, rightEndPosition);}}
            leftStartPosition = tempInstructions.left;
            rightStartPosition = tempInstructions.right;
            leftEndPosition = tempInstructions.left;
            rightEndPosition = tempInstructions.right;}
        else {
            leftEndPosition = tempInstructions.left;
            rightEndPosition = tempInstructions.right;}
        lastOperation = tempInstructions.operation;
        if (i == instructionInfoLength - 1) {
            if (leftEndPosition - leftStartPosition > 0) {;}
            else
                leftEndPosition = -1;
                if (rightEndPosition - rightStartPosition > 0) lines_arr[length++] = DiffLines(lastOperation, leftStartPosition, leftEndPosition, rightStartPosition, rightEndPosition);
                else {
                    rightEndPosition = -1;
                    lines_arr[length++] = DiffLines(lastOperation, leftStartPosition, leftEndPosition, rightStartPosition, rightEndPosition);}}}
    return length;
}

void outputDiff(LineDetails info_arr[], int info_arr_length, char* arr1[], char* arr2[])
{
    for (int i = 0; i < info_arr_length; i++) {
        LineDetails tempDetail = info_arr[i];
        if (tempDetail.leftEndPos != -1)    printf("%i,%i%c", tempDetail.leftStartPos, tempDetail.leftEndPos, tempDetail.operation);
        else    printf("%i%c", tempDetail.leftStartPos, tempDetail.operation);
        if (tempDetail.rightEndPos != -1)   printf("%i,%i\n", tempDetail.rightStartPos, tempDetail.rightEndPos);
        else    printf("%i\n", tempDetail.rightStartPos);
        if (tempDetail.rightEndPos == -1)   tempDetail.rightEndPos = tempDetail.rightStartPos;
        if (tempDetail.leftEndPos == -1)    tempDetail.leftEndPos = tempDetail.leftStartPos;
        switch ((char)tempDetail.operation) {
            case 'a':
                for (int j = tempDetail.rightStartPos; j <= tempDetail.rightEndPos; j++)
                    printf("> %s\n", arr1[j - 1]);
                break;
            case 'd':   
                for (int j = tempDetail.leftStartPos; j <= tempDetail.leftEndPos; j++)
                    printf("< %s\n", arr2[j - 1]);
                break;
            case 'c':
                for (int j = tempDetail.leftStartPos; j <= tempDetail.leftEndPos; j++) printf("< %s\n", arr2[j - 1]);
                printf("---\n");
                for (int j = tempDetail.rightStartPos; j <= tempDetail.rightEndPos; j++) printf("> %s\n", arr1[j - 1]);
                break;}}
}

void freeMemory(char* arr[], int lineCount)
{
    for (int i = 0; i < lineCount; i++) {
        if (arr[i] == NULL) return;
        free(arr[i]);}
}