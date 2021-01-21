#include "../structs/Hashmap.h"

struct Settings {
	int boardX;
	int boardY;
	char* player1;
	int player1Size; //will be used to output the player names, we need to give the amount of chars to be saved
	char* player2;
	int player2Size;
	bool solo;
	int depth;
};

struct Move {
	int column;
	int token;
};