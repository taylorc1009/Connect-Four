#include <ctype.h>
#include "game.h"

#define NAME_BUFFER_MAX 32 //2 extra bytes to accomodate '\n' and '\0'

int removeExcessSpaces(char* str);
int getName(char** player);
void setup(struct Settings* settings);
int main(int argc, char** argv);