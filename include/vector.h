#ifndef VECTOR_H
#define VECTOR_H

#define INIT_CAPACITY 4

typedef struct vector {
    void **items; //for storing any type
    int capacity;
    int total;
} vector;

void vectorInit(vector *);
int vectorTotal(vector *);
static void vectorResize(vector *, int);
void vectorAdd(vector *, char *,int );
void vectorSet(vector *, int, void *);
void * vectorGet(vector *, int);
void vectorRemove(vector *, int);
void vectorFree(vector *);

#endif