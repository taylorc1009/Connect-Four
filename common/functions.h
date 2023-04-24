#include <time.h>
#include "types.h"

void delay(int numOfSeconds);
void inline cleanStdin();
int getUserInputInRange(const int min, const int max, const bool inPlay);
struct Matrix* checkWin(const int row, const int column, const struct Hashmap* restrict board, const int token, const bool isAICheck);
