#include "Matrix.h"

struct Matrix* createMatrix(int x, int y) {
    if (x < 0 || y < 0)
        return NULL;
    
    struct Matrix* m = (struct Matrix*)malloc(sizeof(struct Matrix));
    m->rows = x;
    m->columns = y;
    m->data = calloc(x * y,sizeof(int));
    
    return m;
}

int* matrixCell(struct Matrix* m, int column, int row) {
    //assert(column < m->columns && row < m->rows); //don't use 'assert' it doesn't exist in GCC; you could use an 'if' to check if the values are out of bounds but that's not really our job, anyone using this data structure should know to check this on their own as it's not something we can fix for them
    return &m->data[row * m->columns + column];
}

void freeMatrix(struct Matrix* m) {
    free(m->data);
    free(m);
}