#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <string.h>

#include <chmod.h>

int executeChmod(int argc, char *argcv[]) {
    if (argc != 2) {
        fprintf(stderr,"Error: Too few/many arguments!\n");
        exit(EXIT_FAILURE);
    }

    if (access(argcv[argc], F_OK) != 0) {
        fprintf(stderr,"File: %s doesn't exists\n", argcv[argc]);
        exit(EXIT_FAILURE);
    }
    resetOptions();
    readOptions(argcv[argc - 1]);

    // chmod(argcv[argc], S_IXUSR | S_IXOTH | S_IXGRP);
    exit(EXIT_SUCCESS);
}

static void resetOptions() {
    OPTION_EXECUTE = 0;
    OPTION_READ = 0;
    OPTION_WRITE = 0;
}

void readOptions(char *options) {
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

    printf("READ = %d\n", OPTION_READ);
    printf("WRITE = %d\n", OPTION_WRITE);
    printf("EXECUTE = %d\n", OPTION_EXECUTE);

}
