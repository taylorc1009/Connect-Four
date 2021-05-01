#include "../structs/Matrix.h"

struct Settings {
	int boardX;
	int boardY;
	char* player1; //change this (and player2) to a 'PlayerName' struct, containing both the char array ('playerX') and the size (int, 'playerXSize')
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