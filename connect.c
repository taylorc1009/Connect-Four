/* TODO
*	Implement:
*	- Randomize (or allow selection) of player or AI turn on first move
*	- system("cls"); after board dimensions change?
*	- Save and load
*	- Test undo and redo more
*	- Test how the 'evaluateWindow' 2player:2empty -=1 affects AI quality
*	
*	Update README.md upon completion (if required)
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include "AI.h"

#define NUL '\0' //used to "nullify" a char

#define NAME_MAX 32 //2 extra bytes to accomodate '\n' and '\0', so remember to subtract 2 during char* length checks

//C console colours (source - https://stackoverflow.com/a/3586005/11136104)
#define P1COL "\x1B[31m" //red
#define P2COL "\x1B[33m" //yellow
#define PNRM "\x1B[0m" //default console text color

struct Settings {
	int boardX;
	int boardY;
	char* player1;
	char* player2;
	bool solo;
	int depth;
};

struct Move {
	int column;
	int token;
};

void setup(struct Settings* settings);
void play(struct Settings* settings);
void displayBoard(struct Hashmap* b);

static inline void cleanStdin() {
	char c = NUL;
	while ((c = getchar()) != '\n' && c != 0) { /* do nothing until input buffer is fully flushed */ }
}

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

void welcome(int x, int y) {
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney\nTo continue, select either:\n\n 1 - how to play + controls\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit\n", x, y);
}

int main(int argc, char** argv) {
	system("cls");

	struct Settings* settings = (struct Settings*) malloc(sizeof(struct Settings));
	settings->boardX = 7;
	settings->boardY = 6;

	welcome(settings->boardX, settings->boardY);
	
	int option;
	do {
		settings->solo = false;
		settings->depth = 0;
		printf("\nWhat would you like to do?\n> ");
		option = validateOption(1, 5, false);

		switch (option) {
			case 1:
				printf("\nConnect 4 is a rather simple game. Both players take a turn each selecting a column\nwhich they would like to drop their token (player 1 = %sRED%s, player 2 = %sYELLOW%s) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally. Here are the in-game controls:\n\n 1-9 = column you wish to place your token in\n 0 = exit\n u = undo\n r = redo \n s = save\n", P1COL, PNRM, P2COL, PNRM);
				break;

			case 2:
				printf("\nPlease enter the width (amount of columns) you want to play with (5-9)\n> ");
				settings->boardX = validateOption(5, 9, false);
				printf("\nPlease enter the height (amount of rows) you want to play with (5-9)\n> ");
				settings->boardY = validateOption(5, 9, false);
				printf("\nBoard dimensions changed successfully to %dx%d\n", settings->boardX, settings->boardY);
				delay(2);
				break;

			case 3:
				setup(settings);
				system("cls");
				welcome(settings->boardX, settings->boardY);
				break;

			case 4:
				settings->solo = true;
				setup(settings);
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

int validateOption(int min, int max, bool inPlay) { //used to validate integers within a given range
	bool valid;
	int num;
	char buffer[3];

	//while (num == -1) { //if a char is entered, invalid input message isn't displayed (only after the first input so is this something to do with the input stream?)
	//	char term;
	//	if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= min && num <= max)) {
	//		printf("%d", num);
	//		if (inPlay && (num == 'r' || num == 'u'))
	//			continue;

	//		printf("\n(!) invalid input: please re-enter an number between %d and %d\n> ", min, max);
	//		/*char c = NUL;
	//		do {
	//			c = getchar();
	//		} while (!isdigit(c));
	//		ungetc(c, stdin);*/
	//		cleanStdin();
	//		num = -1;
	//	}
	//}

	do {
		fgets(buffer, sizeof buffer, stdin);

		num = (int)buffer[0];
		bool juncture = buffer[1] == '\n';

		valid = (((num >= min + '0' && num <= max + '0') || (inPlay && (num == 'r' || num == 'u' || num == 's'))) && juncture);
		if (!valid) {
			if (!juncture)
				cleanStdin();
			printf("\n(!) invalid input: please re-enter an number between %d and %d\n> ", min, max);
		}
	} while (!valid);

	return num - '0';
}

void removeExcessSpaces(char* str) { //used to remove preceding and exceding spaces from strings
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

void getName(char** player) { //gets a player name and dynamically resizes the allocation of the char array based on the input
	char* buffer = (char*)malloc(sizeof(char) * NAME_MAX);
	memset(buffer, NUL, NAME_MAX);

	char* input = NULL;
	while (input == NULL) {
		input = fgets(buffer, NAME_MAX, stdin);
		removeExcessSpaces(buffer);
		size_t bufLen = strlen(buffer);

		if (buffer == 0 || buffer[0] == '\n' || buffer[0] == '\0') {
			printf("\n(!) name empty, please enter one\n> ");
			input = NULL;
		} 
		else if (buffer[bufLen - 1] != '\n') {
			printf("\n(!) name is too long (30 characters max), please re-enter\n> ");
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
		settings->player2 = "AI"; //this makes the char* static so no point dynamically allocating before
		printf("\nWelcome %s%s%s! Which difficulty level would you like to play at?\n\n 1 - easy\n 2 - medium\n 3 - hard\n 4 - expert\n\n> ", P1COL, settings->player1, PNRM);
		switch (validateOption(1, 4, false)) {
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

bool undo(struct Hashmap** board, struct Hashmap** history) {
	struct Stack* moveStack = hashGet(*history, 0);
	struct Stack* undoStack = hashGet(*history, 1);
	struct Move* undoMove = (struct Move*)malloc(sizeof(struct Move));
	memcpy(undoMove, (struct Move*)stackGet(moveStack, moveStack->top), sizeof(struct Move));

	if (!pop(moveStack))
		return true;

	resizeStack(moveStack, -1);

	resizeStack(undoStack, 1);
	push(undoStack, &undoMove);

	return !pop(hashGet(*board, undoMove->column));
}

bool redo(struct Hashmap** board, struct Hashmap** history) {
	struct Stack* moveStack = hashGet(*history, 0);
	struct Stack* undoStack = hashGet(*history, 1);

	if (!undoStack->size)
		return true;

	struct Move* redoMove = (struct Move*)malloc(sizeof(struct Move));
	memcpy(redoMove, (struct Move*)stackGet(undoStack, undoStack->top), sizeof(struct Move));

	if (!pop(undoStack))
		return true;

	resizeStack(undoStack, -1);

	resizeStack(moveStack, 1);
	push(moveStack, &redoMove);

	int* tok = malloc(sizeof(int));
	*tok = redoMove->token;
	return addMove(*board, redoMove->column, tok);
}

void updateHistory(struct Hashmap** history, int column, int p) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = column;
	move->token = p;

	resizeStack(hashGet(*history, 0), sizeof(struct stackNode));
	push(hashGet(*history, 0), &move);

	struct Stack* undoStack = hashGet(*history, 1);
	if (undoStack->size) {
		freeStack(undoStack);
		undoStack->size = 0;
		undoStack->top = -1;
	}
}

void play(struct Settings* settings) {
	int x = settings->boardX, y = settings->boardY, p, column = 1;
	bool failedOperation, boardFull = false , win = false, p1ToPlay = true, traversing = false;
	char* curPlayer = NUL;
	char* col;
	int centres[2]; //you could maybe move this to the play method instead, to save processing time as these will be constants during the game
	//we need to determine if there is a literal center column, based on the board dimensions (x will be odd if there is)
	//if there isn't then we will evaluate the 2 centre columns (StackOverflow claims (x & 1) is faster at determining an odd number?)
	if (settings->solo) {
		if (x % 2) {
			//is odd
			centres[0] = (int)round(x / 2.0f) - 1;
			centres[1] = 0; //we will use this to skip the double centre columns check
		}
		else {
			//is even
			centres[0] = (x / 2) - 1;
			centres[1] = centres[0] + 1;
		}
	}

	/*The board structure is made of a list of stacks stored in a hashmap.
	* This is due to the play style of connect 4; a player must only pick
	* a column to drop a token in, this corresponds to the key of the
	* hashmap, and they can only interact with the position on top of the
	* columns - hence the stack implementation. So, ideally, all we would
	* need to do is give the structure a column to play in and a token to
	* push to that column.*/
	struct Hashmap* board = createTable(x, y);

	struct Hashmap* history = createTable(2, 0); //keys: 0 = history of moves made, 1 = history of moves undone

	do {
		displayBoard(board);
		printf("\n\n");

		if (curPlayer != NUL && !traversing) { //used to skip checks before the first initial move, otherwise null issues occur
			//make the 4 connected tokens turn green?
			win = checkWin(hashGet(board, column - 1)->top, column - 1, board, p);
			boardFull = isBoardFull(board, x);
		}

		if (win || boardFull) { //this check is up here and not at the end so we can see the winning move being made
			win ? printf("Congratulations %s%s%s, you win!\n", col, curPlayer, PNRM) : printf("The board is full... Game over!\n");
			delay(2);
			printf("Returning to the main menu...");
			delay(3);
			column = 0; //used instead of 'break' as we're at the end of the loop after this anyway and we still need to deallocate the board
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

			if (!p1ToPlay && settings->solo) { //get the AI to make a move
				if (traversing) {
					printf("(!) AI move held - your previous move was to undo/redo, do you wish to continue doing so?\n(0 to cancel this undo, other controls are the regular undo controls)\n\n> ");
					int cont = validateOption(0, 0, true);
					if (cont == 0)
						traversing = false;
				}
				if (!traversing) {
					printf("%s%s%s is making a move...", col, settings->player2, PNRM);
					AIMakeMove(board, &column, centres, settings->depth);
					int* tok = malloc(sizeof(int));
					*tok = PLAYER_2_TOKEN;
					addMove(board, column - 1, tok); //shouldn't return a full column as we determine this in the AI
					//delay(3); //use this during debugging
				}
			}
			else {
				traversing = false;

				printf("Make your move %s%s%s, select a column number (0 to save and exit)\n> ", col, curPlayer, PNRM);

				do {
					failedOperation = false;
					column = validateOption(0, x, true);
					int toChar = column + '0';

					if (column == 0) {
						printf("\n(!) game closed");
						delay(2); //again, after this, we're at the end of the loop again so there's no need to break
					}
					else if (toChar == 'u') {
						failedOperation = undo(&board, &history);
						if (failedOperation)
							printf("\n(!) board is empty; no possible moves to undo, please try something else\n> ");
						else
							traversing = true;
					}
					else if (toChar == 'r') {
						failedOperation = redo(&board, &history);
						if (failedOperation)
							printf("\n(!) there are no moves to redo, please try something else\n> ");
						else
							traversing = true;
					}
					else if (toChar == 's') {

					}
					else { //implement ctrl+Z and ctrl+Y as undo & redo?
						int* tok = malloc(sizeof(int));
						*tok = p;
						failedOperation = addMove(board, column - 1, tok);
						if (failedOperation)
							printf("\n(!) column full, please choose another\n> ");
						else
							updateHistory(&history, column - 1, p);
					}
				} while (failedOperation);
			}
			p1ToPlay = !p1ToPlay;
		}
	} while ((column >= 1 && column <= x) || traversing);

	freeHashmap(board);
	freeHashmap(history);
}

void displayBoard(struct Hashmap* board) { //add a move down animation?
	int x = getX(board), y = getY(board), i, j;
	system("cls");

	for (i = 0; i < y; i++) {
		printf("+");
		for (j = 0; j < x; j++)
			printf("---+");
		printf("\n");
		printf("|");
		for (j = 0; j < x; j++) {
			int p = *((int*)getToken(board, j, (y - 1) - i)); //'(y - 1) - i' fixes the display, otherwise it would come out upside down

			if (p) {
				char* col = PNRM; //initialise as PNRM in case we somehow don't get a colour, will prevent crashing
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
	for (i = 0; i < x; i++)
		printf("---+");
	printf("\n");

	for (i = 1; i < x + 1; i++)
		printf("  %d ", i);
	printf("\n");
}

bool checkWin(int row, int column, struct Hashmap* board, int p) {
	int x = getX(board), y = getY(board);
	int* tok;
	
	//horizontal check
	int count = 0;
	for (int i = (column - 3 < 0 ? 0 : column - 3); i < (column + 4 > x ? x : column + 4); i++) {
		if (*((int*)getToken(board, i, row)) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	//vertical check
	count = 0;
	for (int i = (row - 3 < 0 ? 0 : row - 3); i < (row + 4 > y ? y : row + 4); i++) {
		if (*((int*)getToken(board, column, i)) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	//bottom-left to top-right diagonal check
	count = 0;
	int i = row, j = column;

	//creates check point to start checking from diagonally
	while (i != 0 && j != 0 && i > row - 3 && j > column - 3) {
		i--;
		j--;
	}

	for (i, j; i < y && j < x && i < row + 4 && j < column + 4; i++, j++) {
		if (*((int*)getToken(board, j, i)) == p) {
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

	//creates check point to start checking from diagonally
	while (i != 0 && j != y && i > row - 3 && j < column + 3) {
		i--;
		j++;
	}

	for (i, j; i < y && j >= 0 && i < row + 4 && j > column - 4; i++, j--) {
		if (*((int*)getToken(board, j, i)) == p) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	return false;
}