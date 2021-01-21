#include <time.h>
#include "types.h"

void delay(int numOfSeconds);
void cleanStdin();
int validateOption(int min, int max, bool inPlay);
int** checkWin(int row, int column, struct Hashmap* board, int token);
