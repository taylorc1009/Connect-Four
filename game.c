#include <math.h>
#include "game.h"

bool undo(struct Hashmap** restrict board, struct Hashmap** restrict history) {//, int* column) {
	struct Stack* moveStack = hashGet(*history, 0);

	if (!moveStack->size)
		return false;

	struct Stack* undoStack = hashGet(*history, 1);
	struct Move* undoMove = (struct Move*)malloc(sizeof(struct Move));

	memcpy(undoMove, (struct Move*)stackGet(moveStack, moveStack->top), sizeof(struct Move));

	if (!pop(hashGet(*board, undoMove->column))) { //if removing (undoing) the move from the column was unsuccessful, prevent move history and undo history manipulation so the move isn't lost
		free(undoMove);
		return false;
	}

	//remove the move, that's being undone, from the move history
	pop(moveStack);
	resizeStack(moveStack, -1);

	//add the move, that's being undone, to the history of undone moves
	resizeStack(undoStack, 1);
	push(undoStack, (void*)undoMove);

	//*column = moveStack->top == -1 ? 1 : ((struct Move*)stackGet(moveStack, moveStack->top))->column + 1; //if the next move to undo is the only remaining move, there's no point updating column as it is now the user's turn again, it will crash if we do anyway as the board is now empty

	return true;
}

bool redo(struct Hashmap** restrict board, struct Hashmap** restrict history) {//, int* column) {
	struct Stack* undoStack = hashGet(*history, 1);

	if (!undoStack->size)
		return false;

	struct Stack* moveStack = hashGet(*history, 0);

	struct Move* redoMove = (struct Move*)malloc(sizeof(struct Move));
	memcpy(redoMove, (struct Move*)stackGet(undoStack, undoStack->top), sizeof(struct Move));

	int* tok = malloc(sizeof(int));
	*tok = redoMove->token;
	if(!addMove(*board, redoMove->column, tok)) { //if adding (redoing) the move to the column was unsuccessful, prevent undo history and move history manipulation so the move isn't lost
		free(redoMove);
		return false;
	}

	//remove the move, that's being redone, from the undo history
	pop(undoStack);
	resizeStack(undoStack, -1);

	//add the move, that's being redone, to the history of moves
	resizeStack(moveStack, 1);
	push(moveStack, (void*)redoMove);

	//*column = redoMove->column + 1;

	return true;
}

void updateHistory(struct Hashmap** restrict history, const int column, const int token) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = column;
	move->token = token;

	resizeStack(hashGet(*history, 0), 1);
	push(hashGet(*history, 0), (void*)move);

	struct Stack* undoStack = hashGet(*history, 1);
	if (undoStack->size) { //if there are redo-able moves, clear them as, since the user has made a new move after undoing, these may not be possible to redo
		freeStack(undoStack);
		undoStack->size = 0;
		undoStack->top = -1;
	}
}

bool doOperation(struct Hashmap** restrict board, struct Hashmap** restrict history, const struct Settings* restrict settings, const int* restrict column, const int token, bool* restrict traversing, bool* restrict saving, const bool turn, const int AIOperator) {
	int toChar = AIOperator == -1 ? *column + '0' : AIOperator + '0';
	bool successfulOperation = true;

	if ((*column == 0) || (*traversing && AIOperator == 0)) {
		*traversing = false;
		if (AIOperator != 0) {
			printf("\n(!) game closed");
		}
	}
	else {
		switch (toChar) {
			case 'u':
				successfulOperation = undo(board, history);// , column);
				if (successfulOperation)
					*traversing = true;
				else
					printf("\n(!) board is empty; no possible moves to undo, please try something else\n> ");
				break;
			case 'r':
				successfulOperation = redo(board, history);// , column);
				if (successfulOperation)
					*traversing = true;
				else
					printf("\n(!) there are no moves to redo, please try something else\n> ");
				break;
			case 's':
				successfulOperation = saveGame(board, history, settings, turn, *traversing);
				if (successfulOperation) {
					printf("\nGame saved!");
					delay(1);
				}
				else //in this case, an error occurred, so let the user read it
					printf("\nPress any key to continue...");
					getc(stdin);
				*saving = true;
				break;
			default:
				*traversing = false;
				int* tok = malloc(sizeof(int));
				*tok = token;
				successfulOperation = addMove((*board), *column - 1, tok);
				if (successfulOperation)
					updateHistory(history, *column - 1, token);
				else {
					free(tok);
					printf("\n(!) column full, please choose another\n> ");
				}
				break;
		}
	}

	return successfulOperation;
}

void displayBoard(const struct Hashmap* restrict board, const struct Matrix* restrict win) {
	int x = getX(board), y = getY(board), i, j, k = 0, l = 0;
	system(CLEAR_TERMINAL);

	for (i = 0; i < y; i++) {
		printf("+");
		for (j = 0; j < x; j++)
			printf("---+");
		printf("\n");
		printf("|");
		for (j = 0; j < x; j++) {
			k = (y - 1) - i; //'(y - 1) - i' fixes the display, otherwise it would come out upside down
			int token = getToken(board, j, k);

			if (token) {
				char* colour = DEFAULT_COLOUR; //initialise as default in case we somehow don't get a colour, will prevent crashing
				if (win && (l >= 0 && l <= 3) && (j == *((int*)matrixCell(win, 0, l)) && k == *((int*)matrixCell(win, 1, l)))) {
					colour = WIN_COLOUR;
					l++;
				}
				else if (token == PLAYER_1_TOKEN)
					colour = PLAYER_1_COLOUR;
				else if (token == PLAYER_2_TOKEN)
					colour = PLAYER_2_COLOUR;
				printf(" %sO%s |", colour, DEFAULT_COLOUR);
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
	printf("\n\n\n");
}

void switchTurn(const bool playerOneToPlay, const struct Settings* settings, bool* restrict saving, char** restrict player, int* restrict token, char** restrict colour) {
	if (!*saving) {
		if (playerOneToPlay) {
			*player = settings->player1;
			*token = PLAYER_1_TOKEN;
			*colour = PLAYER_1_COLOUR;
		}
		else {
			*player = settings->player2;
			*token = PLAYER_2_TOKEN;
			*colour = PLAYER_2_COLOUR;
		}
	}
	else
		*saving = false;
}

void play(struct Hashmap** restrict loadedBoard, struct Hashmap** restrict loadedHistory, const struct Settings* restrict settings, const bool loadedTurn, const bool loadedTraversing) {
	int x = settings->boardX, y = settings->boardY, token, column = 1;
	bool successfulOperation, boardFull = false, playerOneToPlay = loadedTurn, traversing = loadedTraversing, saving = false;
	char* player = NULL;
	char* colour;
	if (settings->solo)
		int centres[2] = {(int)round(x / 2.0f) - 1, x % 2 ? 0 : centres[0] + 1};
	struct Hashmap* board;
	struct Hashmap* history;
	struct Matrix* win = NULL;

	board = loadedBoard ? *loadedBoard : createTable(x, y);
	history = loadedHistory ? *loadedHistory : createTable(2, 0); //keys: 0 = history of moves made, 1 = history of moves undone
	//column = getToken(history, 0, hashGet(history, 0)->top) + 1;

	do {
		if (player != NULL && !traversing && !saving) { //used to skip checks before the first move (including after a game has been loaded), otherwise null issues occur
			win = checkWin(hashGet(board, column - 1)->top, column - 1, board, token, false);
			if (win == NULL)
				boardFull = isBoardFull(board, x);
		}

		displayBoard(board, win);
		if (win || boardFull) { //this check is up here and not at the end so we can see the winning move being made
			win ? printf("Congratulations %s%s%s, you win!", colour, player, DEFAULT_COLOUR) : printf("The board is full... Game over!"); //check for a win instead of board full in case a player won on the last available move
			column = 0; //used instead of 'break' as we're at the end of the loop after this anyway and we still need to deallocate the board
		}
		else {
			switchTurn(playerOneToPlay, settings, &saving, &player, &token, &colour);

			if (!playerOneToPlay && settings->solo) { //get the AI to make a move
				if (traversing) {
					printf("(!) %s%s%s move held - your previous move was to undo/redo, do you wish to continue doing so?\n    (enter 0 to cancel this operation, other controls are the regular undo/redo controls)\n\n> ", colour, settings->player2, DEFAULT_COLOUR);
					
					do {
						int operation = validateOption(0, 0, true); //we use a separate identifier here ('operation') as 'column' is used to get the column which the undo/redo is made in during the AI hold
						successfulOperation = doOperation(&board, &history, settings, &column, token, &traversing, &saving, playerOneToPlay, operation);
					} while (!successfulOperation);

					if (!traversing)
						printf("\n");
				}
				if (!traversing) { //this looks like it could be an "else" to the above "if" but it can't; if "doOperation", in the above "if", sets 'traversing' to false then that means the user cancelled undoing/redoing an AI move, at which point 'column' would also be 0 and (if this line is an else) the code below won't execute as the code above did, so the AI never makes a move and thus the game is closed
					printf("%s%s%s is making a move...", colour, settings->player2, DEFAULT_COLOUR);

					AIMakeMove(board, &column, centres, settings->depth); //give board by value so we don't accidentally edit it

					int* tok = malloc(sizeof(int));
					*tok = PLAYER_2_TOKEN;
					if (!addMove(board, column - 1, tok)) //shouldn't return a full column as we determine this in the AI (if we didn't, we'd need to run Minimax twice from here which is a bad (slow) idea), but just in case, deallocate the memory if this happens
						free(tok);

					updateHistory(&history, column - 1, token);
				}
			}
			else {
				printf("Make your move %s%s%s:\n> ", colour, player, DEFAULT_COLOUR);

				do {
					column = validateOption(0, x, true);
					successfulOperation = doOperation(&board, &history, settings, &column, token, &traversing, &saving, playerOneToPlay, -1);
				} while (!successfulOperation);
			}

			if (!saving)
				playerOneToPlay = !playerOneToPlay;
		}
	} while ((column >= 1 && column <= x) || traversing || saving);

	printf("\nPress any key to continue...");
	getc(stdin);

	freeHashmap(board);
	freeHashmap(history);

	if (win)
		freeMatrix(win);
}