#include "Hashmap.h"

struct Matrix{
    int rows;
    int columns;
    int *data;
};

struct Matrix* createMatrix(int x, int y);
int* matrixCell(struct Matrix* m, int column, int row);
void freeMatrix(struct Matrix* m);