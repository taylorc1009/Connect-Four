/* TODO
*	Implement:
*	- Randomize (or allow selection) of bor or AI start
*	- system("cls"); after board dimensions change?
*	- Save and load
*	- Undo and redo
*	- Full board (no empty columns) detection
*	
*	Update README.md upon completion
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "AI.h"

#define NUL '\0' // used to "nullify" a char

#define NAME_MAX 32 // 2 extra bytes to accomodate '\n' and '\0', so remember to subtract 2 during char* length checks

// C console colours (source - https://stackoverflow.com/a/3586005/11136104)
#define P1COL "\x1B[31m" // red
#define P2COL "\x1B[33m" // yellow
#define PNRM "\x1B[0m" // default console text color

struct Settings {
	int boardX;
	int boardY;
	char* player1;
	char* player2;
	bool solo;
	int depth;
};

void setup(struct Settings* settings);
void play(struct Settings* settings);
void displayBoard(struct hashmap* b);
void freeBoard(struct hashmap* board);

static inline void cleanStdin() {
	char c = NUL;
	while ((c = getchar()) != '\n' && c != EOF) { /* do nothing until input buffer is fully flushed */ }
}

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

void welcome(int x, int y) {
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney\nTo continue, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit\n", x, y);
}

int main(int argc, char** argv) {
	system("cls");

	struct Settings* settings = (struct Settings*) malloc(sizeof(struct Settings));
	settings->boardX = 7;
	settings->boardY = 6;

	welcome(settings->boardX, settings->boardY);
	
	int option;// = 0;
	do {
		settings->solo = false;
		settings->depth = 0;
		printf("\nWhat would you like to do?\n> ");
		option = validateOption(1, 5);

		switch (option) {
			case 1:
				printf("\nConnect 4 is a rather simple game. Both players take a turn each selecting a column\nwhich they would like to drop their token (player 1 = %sRED%s, player 2 = %sYELLOW%s) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally.\n", P1COL, PNRM, P2COL, PNRM);
				//option = 0;
				break;

			case 2:
				printf("\nPlease enter the width (amount of columns) you want to play with (6-12)\n> ");
				settings->boardX = validateOption(6, 12);
				printf("\nPlease enter the height (amount of rows) you want to play with (6-12)\n> ");
				settings->boardY = validateOption(6, 12);
				printf("\nBoard dimensions changed successfully to %dx%d\n", settings->boardX, settings->boardY);
				delay(2);
				//option = 0;
				break;

			case 3:
				setup(settings);
				//option = 0;
				system("cls");
				welcome(settings->boardX, settings->boardY);
				break;

			case 4:
				settings->solo = true;
				setup(settings);
				//option = 0;
				system("cls");
				welcome(settings->boardX, settings->boardY);
				break;

			case 5:
				free(settings);
				system("cls");
				printf("Connect 4 closed, goodbye!\n");
				break;
		}
	} while (option != 5);
	return 0;
}

int validateOption(int min, int max) { // used to validate integers within a given range
	bool valid = false;
	int num = -1; // use -1 as the minimum to allow 0 to be entered during the game

	while (num == -1) { //if a char is entered, invalid input message isn't displayed (only after the first input so is this something to do with the input stream?)
		char term;
		if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= min && num <= max)) {
			printf("\n! invalid input: please re-enter an number between %d and %d\n> ", min, max);
			char c = NUL;
			do {
				c = getchar();
			} while (!isdigit(c));
			ungetc(c, stdin);
			num = -1;
		}
	}
	return num;
}

void removeExcessSpaces(char* str) { // used to remove preceding and exceding spaces from strings
	int i, j;

	for (i = j = 0; str[i]; ++i)
		if (str[i] == '\n' || (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1]))))
			str[j++] = str[i];

	if (str[j - 1] != '\n') {
		if (isspace(str[j - 1]))
			str[j - 1] = '\n';
		str[j] = '\0';
	}
	else if (isspace(str[j - 2])) {
		str[j - 2] = '\n';
		str[j - 1] = '\0';
	}
}

void getName(char** player) { // gets a player name and dynamically resizes the allocation of the char array based on the input
	char* buffer = (char*)malloc(sizeof(char) * NAME_MAX);
	memset(buffer, NUL, NAME_MAX);

	char* input = NULL;
	while (input == NULL) {
		input = fgets(buffer, NAME_MAX, stdin);
		removeExcessSpaces(buffer);
		size_t bufLen = strlen(buffer);

		if (buffer == 0 || buffer[0] == '\n' || buffer[0] == '\0') {
			printf("\n! name empty, please enter one\n> ");
			input = NULL;
		} 
		else if (buffer[bufLen - 1] != '\n') {
			printf("\n! name is too long (30 characters max), please re-enter\n> ");
			cleanStdin();
			input = NULL;
		} else {
			if (input[bufLen - 1] == '\n') {
				input[bufLen - 1] = '\0';
				bufLen--;
			}
			//removeExcessSpaces(input); //might be redundant due to removeExcessSpaces above

			if (bufLen > sizeof(*player))
				*player = (char*)realloc(*player, sizeof(char) * bufLen);

			strcpy(*player, input);
		}
	}
	free(buffer);
}

void setup(struct Settings* settings) {
	printf("\n%sPlayer 1%s, please enter your name\n> ", P1COL, PNRM);
	settings->player1 = (char*)malloc(sizeof(char) * NAME_MAX);
	getName(&(settings)->player1);

	if (settings->solo) {
		settings->player2 = "Bot"; // this makes the char* static so no point dynamically allocating before
		printf("\nWelcome %s%s%s! Which difficulty level would you like to play at?\n\n 1 - easy\n 2 - medium\n 3 - hard\n 4 - expert\n\n> ", P1COL, settings->player1, PNRM);
		switch (validateOption(1, 4)) {
			case 1:
				settings->depth = 1;
				break;
			case 2:
				settings->depth = 3;
				break;
			case 3:
				settings->depth = 5;
				break;
			case 4:
				settings->depth = 7;
				break;
		}
	}
	else {
		printf("\n%sPlayer 2%s, please enter your name\n> ", P2COL, PNRM);
		settings->player2 = (char*)malloc(sizeof(char) * NAME_MAX);
		getName(&(settings)->player2);
		printf("\nWelcome %s%s%s and %s%s%s!", P1COL, settings->player1, PNRM, P2COL, settings->player2, PNRM);
		delay(1);
	}

	printf("\nStarting...");
	delay(2);
	play(settings);

	free(settings->player1);
	if (!settings->solo)
		free(settings->player2);
}

void play(struct Settings* settings) {
	int x = settings->boardX, y = settings->boardY, p, column = 1;
	bool columnFull, boardFull = false , win = false, p1ToPlay = true;
	char* curPlayer = NUL;
	char* col;
	int centres[2]; //you could maybe move this to the play method instead, to save processing time as these will be constants during the game
	// we need to determine if there is a literal center column, based on the board dimensions (x will be odd if there is)
	// if there isn't then we will evaluate the 2 centre columns (StackOverflow claims (x & 1) is faster at determining an odd number?)
	if (settings->solo) {
		if (x % 2) {
			// is odd
			centres[0] = (int)round(x / 2.0f) - 1;
			centres[1] = 0; // we will use this to skip the double centre columns check
		}
		else {
			// is even
			centres[0] = (x / 2) - 1;
			centres[1] = centres[0] + 1;
		}
	}

	/*The board structure is made of a list of stacks stored in a hashmap.
	* This is due to the play style of connect 4; a player must only pick
	* a column to drop a token in, this corresponds to the key of the
	* hashmap, and they can only interact with the position on top of the
	* columns, hence the stack implementation. So, ideally, all we would
	* need to do is give the structure a column to play in and a token to
	* push to that column.*/
	struct hashmap* board = createTable(x, y);

	do {
		displayBoard(board);
		printf("\n\n");

		// used to skip checks before the first initial move, otherwise null issues occur
		if (curPlayer != NUL) {
			//make the 4 connected tokens turn green?
			//change the checks to only check tokens up to 3 before and 3 after
			win = checkWin(hashGet(board, column - 1)->top, column - 1, board, p);
			boardFull = isBoardFull(board, x);
		}

		if (win || boardFull) { // this check is up here and not at the end so we can see the winning move being made
			win ? printf("Congratulations %s%s%s, you win!", col, curPlayer, PNRM) : printf("The board is full... Game over!");
			delay(2);
			printf("\nReturning to main menu...");
			delay(3);
			column = 0;
			//break;
		}
		else {
			if (p1ToPlay) {
				curPlayer = settings->player1;
				p = PLAYER_1_TOKEN;
				col = P1COL;
			}
			else {
				curPlayer = settings->player2;
				p = PLAYER_2_TOKEN;
				col = P2COL;
			}

			if (!p1ToPlay && settings->solo) { // get the AI to make a move
				printf("%s%s%s is making a move...", col, settings->player2, PNRM);
				AIMakeMove(board, &column, centres, settings->depth);
				addMove(board, column - 1, PLAYER_2_TOKEN); // shouldn't return a full column as we determine this in the AI
				//delay(3); //use this during debugging
			}
			else {
				printf("Make your move %s%s%s, select a column number (0 to save and exit)\n> ", col, curPlayer, PNRM);

				do {
					columnFull = false;
					column = validateOption(0, x);

					if (column == 0) {
						printf("\n! game closed");
						delay(2);
						//break;
					}
					else { //implement ctrl+Z and ctrl+Y as undo & redo?
						columnFull = addMove(board, column - 1, p);
						if (columnFull)
							printf("\n! column full, please choose another\n> ");
					}
				} while (columnFull);
			}
			p1ToPlay = !p1ToPlay;
		}
	} while (column >= 1 && column <= x);

	freeBoard(board);
}

void displayBoard(struct hashmap* board) { //add a move down animation?
	int x = getX(board), y = getY(board), i, j;
	system("cls");

	for (i = 0; i < y; i++) {
		printf("+");
		for (j = 0; j < x; j++)
			printf("---+");
		printf("\n");
		printf("|");
		for (j = 0; j < x; j++) {
			
			// the board is actually stored upside down, "(y - 1) - i" fixes the display
			int p = getToken(board, j, (y - 1) - i);

			if (p) {
				char* col = PNRM; // initialise as PNRM in case we somehow don't get a colour, will prevent crashing
				if (p == PLAYER_1_TOKEN)
					col = P1COL;
				else if (p == PLAYER_2_TOKEN)
					col = P2COL;
				printf(" %sO%s |", col, PNRM);
			}
			else
				printf("   |");
		}
		printf("\n");
	}

	printf("+");
	for (j = 0; j < x; j++)
		printf("---+");
	printf("\n");

	for (j = 1; j < x + 1; j++)
		if(j < 10)
			printf("  %d ", j);
		else
			printf(" %d ", j);
	printf("\n");
}

bool checkWin(int row, int column, struct hashmap* board, int p) {
	int x = getX(board), y = getY(board);

	// horizontal check
	int count = 0;
	for (int i = 0; i < x; i++) {
		if (getToken(board, i, row) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	// vertical check
	count = 0;
	for (int i = 0; i < y; i++) {
		if (getToken(board, column, i) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	// bottom-left to top-right diagonal check
	count = 0;
	int i = row, j = column;

	// creates check point to start checking from diagonally
	while (i != 0 && j != 0) {
		i--;
		j--;
	}

	for (i, j; i < y && j < x; i++, j++) {
		if (getToken(board, j, i) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	// bottom-right to top-left diagonal check
	count = 0;
	i = row;
	j = column;

	// creates check point to start checking from diagonally
	while (i != 0 && j != y) {
		i--;
		j++;
	}

	for (i, j; i < y && j >= 0; i++, j--) {
		if (getToken(board, j, i) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	return false;
}