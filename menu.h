#include <ctype.h>
#include "game.h"

int removeExcessSpaces(char* restrict str);
char* inputString(FILE* restrict fp);
void getPlayerName(char** restrict name, int* restrict nameSize, const int playerNum);
void setup(struct Settings* restrict settings);
void welcome(const int x, const int y);
int main(int argc, char** argv);