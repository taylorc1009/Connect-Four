#include <ctype.h>
#include "game.h"

int removeExcessSpaces(char* str);
char* inputString(FILE* fp);
void getPlayerName(char** name, int* nameSize, int playerNum);
void setup(struct Settings* settings);
int main(int argc, char** argv);