#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>
#include <string.h>

#include "chmod.h"

static void output_permissions(mode_t m)
{
    putchar( m & S_IRUSR ? 'r' : '-' );
    putchar( m & S_IWUSR ? 'w' : '-' );
    putchar( m & S_IXUSR ? 'x' : '-' );
    putchar( m & S_IRGRP ? 'r' : '-' );
    putchar( m & S_IWGRP ? 'w' : '-' );
    putchar( m & S_IXGRP ? 'x' : '-' );
    putchar( m & S_IROTH ? 'r' : '-' );
    putchar( m & S_IWOTH ? 'w' : '-' );
    putchar( m & S_IXOTH ? 'x' : '-' );
    putchar('\n');
}

int executeChmod(int argc, char *argcv[]) {
    if (argc != 2) {
        fprintf(stderr,"Error: Too few/many arguments!\n");
        exit(EXIT_FAILURE);
    }

    if (access(argcv[argc], F_OK) != 0) {
        fprintf(stderr,"File: %s doesn't exists\n", argcv[argc]);
        exit(EXIT_FAILURE);
    }
    resetChmodOptions();
    readChmodOptions(argcv[argc - 1]);
    writeChmodOptions(argcv[argc]);

    exit(EXIT_SUCCESS);
}

void resetChmodOptions() {
    OPTION_EXECUTE = 0;
    OPTION_READ = 0;
    OPTION_WRITE = 0;
}

void readChmodOptions(char *options) {
    int options_length = strlen(options);
    int current_state = 0;
    for (int i = 0; i < options_length; ++i) {
        switch (options[i]) {
        case '-':
            current_state = -1;
            break;
        case '+':
            current_state = 1;
            break;
        case 'r':
            OPTION_READ = current_state;
            break;
        case 'w':
            OPTION_WRITE = current_state;
            break;
        case 'x':
            OPTION_EXECUTE = current_state;
            break;
        default:
            fprintf(stderr, "chmod: invalid mode: \'%c\'\n", options[i]);
            exit(EXIT_FAILURE);
        }
    }
}

void writeChmodOptions(char *destination) {

    if (OPTION_EXECUTE == 1) setFileExecute(destination, &fs);
    if (OPTION_EXECUTE == -1) clearFileExecute(destination, &fs);
    if (OPTION_READ == 1) setFileRead(destination, &fs);
    if (OPTION_READ == -1) clearFileRead(destination, &fs);
    if (OPTION_WRITE == 1) setFileWrite(destination, &fs);
    if (OPTION_WRITE == -1) clearFileWrite(destination, &fs);
}

void setFileRead(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode | (S_IRGRP + S_IROTH + S_IRUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}

void clearFileRead(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode & ~(S_IRGRP + S_IROTH + S_IRUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}

void setFileWrite(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode | (S_IWGRP + S_IWOTH + S_IWUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}

void clearFileWrite(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode & ~(S_IWGRP + S_IWOTH + S_IWUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}

void setFileExecute(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode | (S_IXGRP + S_IXOTH + S_IXUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}

void clearFileExecute(char * destination, struct stat *fs) {
    stat(destination, fs);

    if (chmod(destination, fs->st_mode & ~(S_IXGRP + S_IXOTH + S_IXUSR)) != 0) {
        fprintf(stderr, "chmod: Something went wrong!\n");
        exit(EXIT_FAILURE);
    }
    stat(destination, fs);
}
