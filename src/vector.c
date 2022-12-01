#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vector.h"

void vectorInit(vector *v) {
    v->capacity = INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
}

int vectorTotal(vector *v)  {
    return  v->total;
}

static void vectorResize(vector *v, int capacity) {
    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void vectorAdd(vector *v, char *item, int size) {
    if (v->capacity == v->total) 
        vectorResize(v, v->capacity + 1);
    // strcpy(v->items[v->total++],item);
    v->items[v->total++] = malloc(sizeof(char) * size);
    memcpy(&v->items[v->total++],&item[0], sizeof(char) * size);
    
}

void vectorSet(vector *v, int index, void *item) {
    if ((index >= 0) && (index < v->total)) 
        v->items[index] = item;
}

void *vectorGet(vector  *v, int index) {
    if ((index >= 0) && (index < v->total)) 
        return v->items[index];
    return NULL;
}

void vectorRemove(vector *v, int index) {
    if ((index < 0) || (index > v->total)) 
        return;
    
    v->items[index] = NULL;

    for (int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }
    v->total--;
    vectorResize(v, v->capacity - 1);
}

void vectorFree(vector *v) {
    free(v->items);
}