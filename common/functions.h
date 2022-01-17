#include <time.h>
#include "types.h"

void delay(int numOfSeconds);
void inline cleanStdin();
int validateOption(const int min, const int max, const bool inPlay);
struct Matrix* checkWin(const int row, const int column, const struct Hashmap* restrict board, const int token);
