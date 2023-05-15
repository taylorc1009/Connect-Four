#include <ctype.h>
#include "game.h"

int removeExcessSpaces(char* restrict str);
char* getStringInput(FILE* restrict fp);
void getPlayerName(char** restrict name, int* restrict nameSize, const int playerNum);
void getAIDifficulty(struct Settings* restrict settings);
void setupGame(struct Settings* restrict settings);
void changeBoardDimensions(struct Settings* settings);
void attemptLoadGame(struct Settings* settings) ;
void welcome(const int x, const int y);
int main(int argc, char** argv);