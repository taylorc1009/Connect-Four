#include "Hashmap.h"

struct Matrix {
    int rows;
    int columns;
    int *data;
};

struct Matrix* createMatrix(const int x, const int y);
int* matrixCell(const struct Matrix* restrict m, const int column, const int row);
void freeMatrix(struct Matrix* restrict m);