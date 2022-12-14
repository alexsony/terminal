#include <stdlib.h>
#include <stdio.h>

#include "diff.h"

#define MAXC 1024       

int storeContent(char ***array_file, FILE *file) {
    char buf[MAXC];
    size_t  nptrs = 2,used = 0;             
   
    /* allocate/validate block holding initial nptrs pointers */
    if (((*array_file) = malloc (nptrs * sizeof *(*array_file))) == NULL) {
        perror ("malloc-lines");
        exit (EXIT_FAILURE);
    }
    
    while (fgets (buf, MAXC, file)) {                 
        size_t len;
        buf[(len = strcspn (buf, "\n"))] = 0;      
        
        if (used == nptrs) {    /* check if realloc of lines needed */
            void *tmp = realloc ((*array_file), (2 * nptrs) * sizeof *(*array_file));
            if (!tmp) {                             /* validate reallocation */
                perror ("realloc-lines");
                break;                              /* don't exit, lines still good */
            }
            (*array_file) = tmp;                            /* assign reallocated block to lines */
            nptrs *= 2;                             /* update no. of pointers allocatd */
        }
        
        if (!((*array_file)[used] = malloc (len + 1))) {
            perror ("malloc-lines[used]");
            break;
        }
        memcpy ((*array_file)[used], buf, len + 1);         /* copy line from buf to lines[used] */
        
        used += 1;  
    }                                
    return used;
}

void processEqualCase(char **array_file_1, char **array_file_2, int no_lines) {
    int start_index = -1, stop_index = -1;
    char *similarity = (char *)malloc(no_lines * sizeof(char));  

    for (int i = 0; i < no_lines; ++i) {
        if (strcmp(array_file_1[i], array_file_2[i]) != 0) 
        { 
            similarity[i] = 'c';
        } else {
            similarity[i] = '0';
        }
    }
    // for (int i = 0; i < no_lines; ++i) printf("%c ", similarity[i]);
    // printf("\n");
    for (int i = 0; i < no_lines; ++i) {
        if (similarity[i] == 'c') {
            start_index = i;
            while (similarity[++i] == 'c'); 
            stop_index = i - 1;
        }
        if(start_index != stop_index) {
            start_index++;
            stop_index++;
            printf("%d,%d%c,%d,%d\n", start_index, stop_index, similarity[start_index - 1], start_index, stop_index);

            for(int j = start_index - 1; j< stop_index; ++j) {
                printf("< %s\n", array_file_1[j]);
            }
            printf("---\n");
            for(int j = start_index - 1; j< stop_index; ++j) {
                printf("> %s\n", array_file_2[j]);
            }   
        } else {
            if ((start_index >= 0) && (stop_index  >= 0)) {
                printf("%d%c%d\n", start_index + 1, similarity[start_index], stop_index + 1);
                printf("< %s\n", array_file_1[start_index]);
                printf("---\n");
                printf("> %s\n", array_file_2[stop_index]);
            }
        }
    }

    free(similarity);
}

int compareFiles(FILE *fptr1, FILE *fptr2) {

    char **array_file_1, **array_file_2, *similarity;   
    int no_lines_1, no_lines_2;
    no_lines_1 = storeContent(&array_file_1, fptr1); 
    no_lines_2 = storeContent(&array_file_2, fptr2); 

    int max = (no_lines_1 > no_lines_2) ? no_lines_1 : no_lines_2;
    int min = (no_lines_1 < no_lines_2) ? no_lines_1 : no_lines_2;
    
    if (no_lines_1 == no_lines_2) processEqualCase(array_file_1, array_file_2, no_lines_1);

    for (int i = 0; i < no_lines_1; i++) {
        free(array_file_1[i]);                            
    }

    free(array_file_1);      
    for (int i = 0; i < no_lines_2; i++) {
        free(array_file_2[i]);                            
    }
    free(array_file_2);      
}

int executeDiff(char *file_1, char *file_2) {
    FILE *fptr1, *fptr2;

    fptr1 = fopen(file_1, "r");
    fptr2 = fopen(file_2, "r");

    if (fptr1 == NULL || fptr2 == NULL)
    {
        printf("\nUnable to open file.\n");
        printf("Please check whether file exists and you have read privilege.\n");
        exit(EXIT_FAILURE);
    }

    compareFiles(fptr1, fptr2);
    fclose(fptr1);
    fclose(fptr2);
}
