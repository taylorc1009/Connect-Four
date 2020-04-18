/* TODO
*	Implement:
*	- AI feature
*	- Save and load
*	- Undo and redo
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "hashmap.h"

#define NAME_MAX 32 // 2 extra bytes to accomodate '\0' and '\n', so remember to subtract 2 during char* length checks

// C console colours (source = https://stackoverflow.com/a/3586005/11136104)
#define P1COL "\x1B[31m" // red
#define P2COL "\x1B[33m" // yellow
#define PNRM "\x1B[0m" // default console text color

struct Settings {
	int boardX;
	int boardY;
	char* player1;
	char* player2;
	bool solo;
};

void setup(struct Settings* settings);
void play(struct Settings* settings);
void displayBoard(int x, int y, struct hashmap* b);
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

void welcome(struct Settings* s) {
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit\n", s->boardX, s->boardY);
}

int main(int argc, char** argv) {
	system("cls");

	struct Settings* settings = (struct Settings*) malloc(sizeof(struct Settings));
	settings->boardX = 7;
	settings->boardY = 6;
	settings->player1 = (char*)malloc(sizeof(char) * NAME_MAX);
	settings->player2 = (char*)malloc(sizeof(char) * NAME_MAX);

	welcome(settings);
	
	int option = 0;
	while (option == 0) {
		settings->solo = false;
		printf("\nWhat would you like to do?\n> ");
		option = validateOption(1, 5);
		switch (option) {
			case 1:
				printf("\nConnect 4 is a rather simple game. Both users take a turn each selecting a column\nwhich they would like to drop their token (player 1 = O, player 2 = X) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally.\n", settings->boardX, settings->boardY);
				option = 0;
				break;

			case 2:
				printf("\nPlease enter the width (amount of columns) you want to play with\n> ");
				settings->boardX = validateOption(4, 12);
				printf("\nPlease enter the height (amount of rows) you want to play with\n> ");
				settings->boardY = validateOption(4, 12);
				printf("\nBoard dimensions changed successfully to %dx%d\n", settings->boardX, settings->boardY);
				option = 0;
				break;

			case 3:
				setup(settings);
				option = 0;
				system("cls");
				welcome(settings);
				break;

			case 4:
				settings->solo = true;
				setup(settings);
				option = 0;
				system("cls");
				welcome(settings);
				break;

			case 5:
				free(settings->player1);
				free(settings->player2);
				free(settings);
				system("cls");
				printf("Connect 4 closed, goodbye!\n");
				break;
		}
	}
	return 0;
}

int validateOption(int min, int max) { // used to validate integers within a given range
	bool valid = false;
	int num = -1;

	while (num == -1) { // if a char is entered, invalid input message isn't displayed (only after the first input so is this something to do with the input stream?)
		char term;
		//num = -1;
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
			//removeExcessSpaces(input); // might be redundant due to removeExcessSpaces above

			if (bufLen > sizeof(*player))
				*player = (char*)realloc(*player, sizeof(char) * bufLen);

			strcpy(*player, input);
		}
	}
	free(buffer);
}

void setup(struct Settings* settings) {
	printf("\n%sPlayer 1%s, please enter your name\n> ", P1COL, PNRM);
	getName(&(settings)->player1);

	if (settings->solo) {
		settings->player2 = "Bot";
		printf("\nWelcome %s%s%s!\n> Starting...", P1COL, settings->player1, PNRM);
	}
	else {
		printf("\n%sPlayer 2%s, please enter your name\n> ", P2COL, PNRM);
		getName(&(settings)->player2);
		printf("\nWelcome %s%s%s and %s%s%s!\n> Starting...", P1COL, settings->player1, PNRM, P2COL, settings->player2, PNRM);
	}

	delay(2);
	play(settings);
}

void play(struct Settings* settings) {
	int column, x, y;
	x = settings->boardX;
	y = settings->boardY;
	column = x;

	bool p1ToPlay = true;
	char* curPlayer = NUL;
	char* col;
	int p;

	struct hashmap* board = createTable(x, y);
	bool full, win;

	do {
		displayBoard(x, y, board);
		printf("\n\n");

		// used to skip checks before the first initial move, otherwise null issues occur
		if (curPlayer != NUL)
			win = checkWin(x, y, hashGet(board, column - 1)->top, column - 1, board, p);
		else
			win = false;

		if (win) {
			freeBoard(board);

			printf("Congratulations %s%s%s, you win!", col, curPlayer, PNRM);
			delay(5);
			column = 0;
			break;
		}
		else {
			if (p1ToPlay) {
				curPlayer = settings->player1;
				p = 1;
				col = P1COL;
			}
			else {
				curPlayer = settings->player2;
				p = 2;
				col = P2COL;
			}

			/*if (!p1ToPlay && settings->solo)
				printf("%s is making a move", settings->player2);
				delay(2);
				AIMakeMove();
			else {*/
			printf("Make your move %s%s%s, select a column number (0 to save and exit)\n> ", col, curPlayer, PNRM);

			do {
				column = validateOption(0, x);

				if (column == 0) {
					freeBoard(board);

					printf("\n! game closed");
					delay(2);
					break;
				}
				else { // implement ctrl+Z and ctrl+Y as undo & redo?
					full = push(hashGet(board, column - 1), p);

					if (full)
						printf("\n! column full, please choose another\n> ");
				}
			} while (full);
			//}
			p1ToPlay = !p1ToPlay;
		}
	} while (column >= 1 && column <= x);
}

void displayBoard(int x, int y, struct hashmap* board) { // add a move down animation?
	int i, j;
	system("cls");

	for (i = 0; i < y; i++) {
		printf("+");
		for (j = 0; j < x; j++)
			printf("---+");
		printf("\n");
		printf("|");
		for (j = 0; j < x; j++) {
			int p = stackGet(hashGet(board, j), (y - 1) - i);
			if (p) {
				char* col;
				if (p == 1)
					col = P1COL;
				else if (p == 2)
					col = P2COL;
				else
					col = PNRM; // shouldn't happen, but will prevent a nullpointer in case it does
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

bool checkWin(int x, int y, int row, int column, struct hashmap* board, int p) {
	// make the 4 connected tokens turn green?

	// horizontal check
	int count = 0;
	for (int i = 0; i < y; i++) {
		if (stackGet(hashGet(board, i), row) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	// vertical check
	count = 0;
	for (int i = 0; i < x; i++) {
		if (stackGet(hashGet(board, column), i) == p) {
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

	for (i, j; i < x && j < y; i++, j++) {
		if (stackGet(hashGet(board, j), i) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	//bottom-right to top-left diagonal check
	count = 0;
	i = row;
	j = column;

	// create check point to start checking from diagonally
	while (i != 0 && j != y) {
		i--;
		j++;
	}

	for (i, j; i < x && j > 0; i++, j--) {
		if (stackGet(hashGet(board, j), i) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	return false;
}

void freeBoard(struct hashmap* board) {
	for (int i = 0; i < board->size; i++) {
		for (int j = 0; j < board->list[i]->stack->size; j++)
			free(board->list[i]->stack->list[j]);
		free(board->list[i]->stack);
		free(board->list[i]);
	}
	free(board->list);
	free(board);
}