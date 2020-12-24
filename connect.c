/* TODO
*	Implement:
*	- Finish internal commentary
*	- Randomize (or allow selection) of player or AI turn on first move
*	- Test save and load more
*	- Test undo and redo more
*	- Test how the 'evaluateWindow' 2player:2empty -=1 affects AI quality
*	- Look at if infinitely long player names are possible (using stdin and realloc)
*	- Add a move down animation? Make the 4 connected tokens turn green?
*	
*	Update README.md upon completion (if required)
*/

#include <string.h>
#include <time.h>
#include <math.h>
#include "Hashmap.h"

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
void play(struct Hashmap** loadedBoard, struct Hashmap** loadedHistory, struct Settings* settings, bool turn);
void displayBoard(struct Hashmap* board);

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
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney\nTo continue, select either:\n\n 1 - how to play + controls\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - load a previous save\n 6 - quit\n", x, y);
}

int main(int argc, char** argv) {
	system("cls");

	struct Settings* settings = (struct Settings*)malloc(sizeof(struct Settings));
	settings->boardX = 7;
	settings->boardY = 6;

	welcome(settings->boardX, settings->boardY);
	
	int option;
	do {
		settings->solo = false;
		settings->depth = 0;

		printf("\nWhat would you like to do?\n> ");
		option = validateOption(1, 6, false);
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
				struct Hashmap* board = NULL;
				struct Hashmap* history = NULL;
				bool turn;

				if (loadGame(&board, &history, settings, &turn)) {
					printf("\nGame loaded!");
					delay(1);
					printf("\nStarting...");
					delay(2);

					play(&board, &history, settings, turn);
				}

				system("cls");
				welcome(settings->boardX, settings->boardY);
				break;

			case 6:
				free(settings);

				system("cls");
				printf("Connect 4 closed, goodbye!\n");
				break;
		}
	} while (option != 6);

	return 0;
}

int validateOption(int min, int max, bool inPlay) { //used to validate integers within a given range - because we now use 'fgets' instead of 'scanf', I need to find a way to get numbers with more than 1 digit safely
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
			printf("\n(!) this name is too long (30 characters max), please re-enter\n> ");
			cleanStdin();
			input = NULL;
		}
		else {
			for (int i = 0; i < bufLen; i++) { //prevents ';' in player's names - having one in their name causes issues with save and load
				if (buffer[i] == ';') {
					printf("\n(!) the name entered contains a ';' - this is a system character, please re-enter\n> ");
					input = NULL;
				}
			}

			if (input == NULL)
				continue;

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
	play(NULL, NULL, settings, true);

	free(settings->player1);
	if (!settings->solo)
		free(settings->player2);
}

bool undo(struct Hashmap** board, struct Hashmap** history, int* column) {
	struct Stack* moveStack = hashGet(*history, 0);

	if (!moveStack->size)
		return true;

	struct Stack* undoStack = hashGet(*history, 1);
	struct Move* undoMove = (struct Move*)malloc(sizeof(struct Move));

	memcpy(undoMove, (struct Move*)stackGet(moveStack, moveStack->top), sizeof(struct Move));

	pop(moveStack);
	resizeStack(moveStack, -1);

	resizeStack(undoStack, 1);
	push(undoStack, &undoMove);

	*column = undoMove->column + 1;

	return !pop(hashGet(*board, undoMove->column));
}

bool redo(struct Hashmap** board, struct Hashmap** history, int* column) {
	struct Stack* undoStack = hashGet(*history, 1);

	if (!undoStack->size)
		return true;

	struct Stack* moveStack = hashGet(*history, 0);

	struct Move* redoMove = (struct Move*)malloc(sizeof(struct Move));
	memcpy(redoMove, (struct Move*)stackGet(undoStack, undoStack->top), sizeof(struct Move));

	if (!pop(undoStack))
		return true;

	resizeStack(undoStack, -1);

	resizeStack(moveStack, 1);
	push(moveStack, &redoMove);

	*column = redoMove->column + 1;

	int* tok = malloc(sizeof(int));
	*tok = redoMove->token;
	return addMove(*board, redoMove->column, tok);
}

void updateHistory(struct Hashmap** history, int column, int p) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = column;
	move->token = p;

	resizeStack(hashGet(*history, 0), 1);
	push(hashGet(*history, 0), &move);

	struct Stack* undoStack = hashGet(*history, 1);
	if (undoStack->size) { //if there are redo-able moves, clear them as, since the user has made a new move after undoing, these may not be possible to redo
		freeStack(undoStack);
		undoStack->size = 0;
		undoStack->top = -1;
	}
}

bool saveGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool turn) {
	FILE* file;

	if (file = fopen("save.bin", "w")) {
		fprintf(file, "%c", settings->boardX + '0');
		fprintf(file, "%c", settings->boardY + '0');
		fprintf(file, ";");

		fprintf(file, "%c", ((int)turn) + '0');
		fprintf(file, ";");

		for(int i = 0; i < strlen(settings->player1); i++)
			fprintf(file, "%c", settings->player1[i]);
		fprintf(file, ";");

		for(int i = 0; i < strlen(settings->player2); i++)
			fprintf(file, "%c", settings->player2[i]);
		fprintf(file, ";");

		fprintf(file, "%c", ((int)settings->solo) + '0');
		fprintf(file, ";");

		fprintf(file, "%c", settings->depth + '0');
		fprintf(file, ";");

		for (int i = 0; i < getX(*board); i++) {
			for (int j = 0; j < getY(*board); j++) 
				fprintf(file, "%c", *((int*)getToken(*board, i, j)) + '0');
			fprintf(file, ";");
		}

		for (int i = 0; i < 2; i++) {
			fprintf(file, "%c", hashGet(*history, i)->size + '0');
			fprintf(file, ";");

			for (int j = 0; j < hashGet(*history, i)->size; j++) {
				struct Move* move = (struct Move*)getToken(*history, i, j);

				fprintf(file, "%c", move->column + '0');
				fprintf(file, "%c", move->token + '0');
				fprintf(file, ";");
			}
		}
		
		fclose(file);
	}
	else {
		printf("\n(!) file to save to was not found: 'save.bin'");
		return false;
	}

	return true;
}

bool loadGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool* turn) {
	FILE* file;
	int step = 0, x = 0, y = 0, bufferSize = 300;
	bool isSecond = false; //we will use this to determine if the current value we're reading is the second of 2 digits, for example, X and Y
	char* buffer = malloc(sizeof(char) * bufferSize);
	struct Move* move;

	if (file = fopen("save.bin", "r")) {
		while (fgets(buffer, bufferSize, file) != NULL) {
			for (int i = 0; i < bufferSize; i++) {

				//this all seems like a very awkward way to read from a file, but I did it this way to safely allow an increase in data that ends up exceeding the buffer size
				//if the size does end up exceeding the buffer size, the parameters used (such as 'step' and 'isSecond' will allow it to continue from the exact same step of
				//the loading process with the next buffer
				if (buffer[i] != -1 && buffer[i] != 0) {
					//printf("%d: %d(%c) | step = %d, x = %d, y = %d\n", i, buffer[i], buffer[i], step, x, y);
					if (buffer[i] == ';') {
						if (step == 6 && x != settings->boardX - 1) {
							x++;
							y = 0;
						}
						else if (step == 8 && y == hashGet(*history, x)->size) {
							if (x == 1)
								return true; //disables further reading of the input buffer after completion
							else {
								step = 7;
								x++;
								y = 0;
							}
						}
						else if (step < 8) {
							step++;
							if (step < 7 || (step == 7 && x != 1))
								x = 0;
						}
					}
					else {
						if (step == 0) {
							if (i == 0) {
								settings->player1 = (char*)malloc(sizeof(char)); //this is not a memory leak, same goes for the other dynamically allocated variables in this list - they will always be NULL or have been previously free'd before this point
								memset(settings->player1, NUL, 1);

								settings->player2 = (char*)malloc(sizeof(char));
								memset(settings->player2, NUL, 1);

								*history = createTable(2, 0);
							}

							if (!isSecond)
								settings->boardX = (int)buffer[i] - '0';
							else {
								settings->boardY = (int)buffer[i] - '0';
								*board = createTable(settings->boardX, settings->boardY);
							}

							isSecond = !isSecond;
						}
						else if (step == 1)
							*turn = (bool)(buffer[i] - '0');
						else if (step == 2) {
							settings->player1 = (char*)realloc(settings->player1, sizeof(char) * x + 1);
							settings->player1[x] = buffer[i];
							settings->player1[x + 1] = '\0';
							x++;
						}
						else if (step == 3) {
							settings->player2 = (char*)realloc(settings->player2, sizeof(char) * x + 1);
							settings->player2[x] = buffer[i];
							settings->player2[x + 1] = '\0';
							x++;
						}
						else if (step == 4)
							settings->solo = (bool)(buffer[i] - '0');
						else if (step == 5)
							settings->depth = (int)buffer[i] - '0';
						else if (step == 6) {
							if (y < settings->boardY && ((int)buffer[i] - '0') != 0) {
								int* tok = malloc(sizeof(int));
								*tok = (int)buffer[i] - '0';
								addMove(*board, x, tok);
							}
						}
						else if (step == 7) {
							resizeStack(hashGet(*history, x), (int)buffer[i] - '0');
						}
						else if (step == 8) {
							if (!isSecond) {
								move = (struct Move*)malloc(sizeof(struct Move));
								move->column = (int)buffer[i] - '0';
							}
							else {
								move->token = (int)buffer[i] - '0';

								push(hashGet(*history, x), &move);
								y++;
							}
							isSecond = !isSecond;
						}
					}
				}
			}
		}
	}
	else {
		printf("\n(!) file to load from was not found: 'save.bin'");
		free(buffer);
		return false;
	}

	free(buffer);
	return true;
}

bool doOperation(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, int* column, int token, bool* traversing, bool* saving, bool turn, int AIOperator) {
	int toChar = AIOperator == -1 ? *column + '0' : AIOperator + '0';
	bool failedOperation = false;

	if ((*column == 0) || (*traversing && AIOperator == 0)) {
		*traversing = false;
		if (AIOperator != 0) {
			printf("\n(!) game closed");
			delay(2);
		}
	}
	else if (toChar == 'u') {
		failedOperation = undo(board, history, column);
		if (failedOperation)
			printf("\n(!) board is empty; no possible moves to undo, please try something else\n> ");
		else
			*traversing = true;
	}
	else if (toChar == 'r') {
		failedOperation = redo(board, history, column);
		if (failedOperation)
			printf("\n(!) there are no moves to redo, please try something else\n> ");
		else
			*traversing = true;
	}
	else if (toChar == 's') {
		if (saveGame(board, history, settings, turn)) {
			printf("\nGame saved!");
			delay(1);
		}
		else
			delay(2);

		*saving = true;
	}
	else {
		*traversing = false;
		int* tok = malloc(sizeof(int));
		*tok = token;
		failedOperation = addMove((*board), *column - 1, tok);
		if (failedOperation)
			printf("\n(!) column full, please choose another\n> ");
		else
			updateHistory(history, *column - 1, token);
	}

	return failedOperation;
}

void play(struct Hashmap** loadedBoard, struct Hashmap** loadedHistory, struct Settings* settings, bool turn) {
	int x = settings->boardX, y = settings->boardY, token, column = 1;
	bool failedOperation, boardFull = false , win = false, p1ToPlay = turn, traversing = false, saving = false;
	char* player = NUL;
	char* colour;
	int centres[2];
	struct Hashmap* board;
	struct Hashmap* history;

	/*The board structure is made of a list of stacks stored in a hashmap.
	* This is due to the play style of connect 4; a player must only pick
	* a column to drop a token in, this corresponds to the key of the
	* hashmap, and they can only interact with the position on top of the
	* columns - hence the stack implementation. So, ideally, all we would
	* need to do is give the structure a column to play in and a token to
	* push to that column.*/
	if (loadedBoard == NULL && loadedHistory == NULL) {
		board = createTable(x, y);
		history = createTable(2, 0); //keys: 0 = history of moves made, 1 = history of moves undone
	}
	else {
		board = *loadedBoard;
		history = *loadedHistory;
	}

	//we need to determine if there is a literal centre column, based on the board dimensions, for the AI scoring (x will be odd if there is)
	//if there isn't, then we will evaluate the 2 centre columns (StackOverflow claims (x & 1) is faster at determining an odd number?)
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

	do {
		displayBoard(board);
		printf("\n\n");

		if (player != NUL && !traversing && !saving) { //used to skip checks before the first initial move, otherwise null issues occur
			win = checkWin(hashGet(board, column - 1)->top, column - 1, board, token);
			boardFull = isBoardFull(board, x);
		}

		if (win || boardFull) { //this check is up here and not at the end so we can see the winning move being made
			win ? printf("Congratulations %s%s%s, you win!\n", colour, player, PNRM) : printf("The board is full... Game over!\n");
			delay(2);
			printf("Returning to the main menu...");
			delay(3);
			column = 0; //used instead of 'break' as we're at the end of the loop after this anyway and we still need to deallocate the board
		}
		else {
			if (!saving) {
				if (p1ToPlay) {
					player = settings->player1;
					token = PLAYER_1_TOKEN;
					colour = P1COL;
				}
				else {
					player = settings->player2;
					token = PLAYER_2_TOKEN;
					colour = P2COL;
				}
			}
			else
				saving = false;

			if (!p1ToPlay && settings->solo) { //get the AI to make a move
				if (traversing) {
					printf("(!) %s%s%s move held - your previous move was to undo/redo, do you wish to continue doing so?\n    (0 to cancel this operation, other controls are the regular undo/redo controls)\n\n> ", colour, settings->player2, PNRM);
					int operation = validateOption(0, 0, true); //we use a separate identifier here ('operation') as 'column' is used to get the column which the undo/redo is made in during the AI hold

					failedOperation = doOperation(&board, &history, settings, &column, token, &traversing, &saving, p1ToPlay, operation);

					if (!traversing)
						printf("\n");
				}
				if (!traversing) {
					printf("%s%s%s is making a move...", colour, settings->player2, PNRM);

					AIMakeMove(board, &column, centres, settings->depth);

					int* tok = malloc(sizeof(int));
					*tok = PLAYER_2_TOKEN;
					addMove(board, column - 1, tok); //shouldn't return a full column as we determine this in the AI

					updateHistory(&history, column - 1, token);
					//delay(3); //use this during debugging
				}
			}
			else {
				printf("Make your move %s%s%s, select a column number (0 to save and exit)\n> ", colour, player, PNRM);

				do {
					failedOperation = false;
					column = validateOption(0, x, true);

					failedOperation = doOperation(&board, &history, settings, &column, token, &traversing, &saving, p1ToPlay, -1);
				} while (failedOperation);
			}
			if (!saving)
				p1ToPlay = !p1ToPlay;
		}
	} while ((column >= 1 && column <= x) || traversing || saving);

	freeHashmap(board);
	freeHashmap(history);
}

void displayBoard(struct Hashmap* board) {
	int x = getX(board), y = getY(board), i, j;
	system("cls");

	for (i = 0; i < y; i++) {
		printf("+");
		for (j = 0; j < x; j++)
			printf("---+");
		printf("\n");
		printf("|");
		for (j = 0; j < x; j++) {
			int token = *((int*)getToken(board, j, (y - 1) - i)); //'(y - 1) - i' fixes the display, otherwise it would come out upside down

			if (token) {
				char* colour = PNRM; //initialise as PNRM in case we somehow don't get a colour, will prevent crashing
				if (token == PLAYER_1_TOKEN)
					colour = P1COL;
				else if (token == PLAYER_2_TOKEN)
					colour = P2COL;
				printf(" %sO%s |", colour, PNRM);
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

bool checkWin(int row, int column, struct Hashmap* board, int token) {
	int x = getX(board), y = getY(board);
	int* tok;
	
	//horizontal check
	int count = 0;
	for (int i = (column - 3 < 0 ? 0 : column - 3); i < (column + 4 > x ? x : column + 4); i++) {
		if (*((int*)getToken(board, i, row)) == token) {
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
		if (*((int*)getToken(board, column, i)) == token) {
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
		if (*((int*)getToken(board, j, i)) == token) {
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
		if (*((int*)getToken(board, j, i)) == token) {
			count++;
			if (count >= 4)
				return true;
		}
		else
			count = 0;
	}

	return false;
}