#include <time.h>
#include "types.h"

void delay(int numOfSeconds);
void cleanStdin();
int validateOption(int min, int max, bool inPlay);
struct Matrix* checkWin(const int row, const int column, const struct Hashmap* restrict board, const int token);
