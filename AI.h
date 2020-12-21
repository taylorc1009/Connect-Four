//source - https://www.youtube.com/watch?v=MMLtza3CZFM&list=WL&index=310&t=863s

#include <math.h>
#include "Hashmap.h"

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

//#define MINIMAX_DEPTH 7 //higher = better but slower AI (more checks are made)

#define ARRAY_LENGTH(x) ((int)sizeof(x) / sizeof((x)[0])) //remember, we cannot calculate the size of a dynamic array, the compiler will never know its size

/*There may still be issues here. I've yet to find more.
*
*I noticed it sometimes does not make a move on getting 3 in a row with 2 free empty slots on 
*one/each end, it will go for a win instead (when this happened, it seemed to be fine as the
*AI win was near)
*
*evaluateWindow score for player 3 in a row used to be different, appeared to work better in
*some cases (check the comment in the method for a little more info)*/

struct Move {
	int column;
	int score;
	bool gameOver;
};

void freeBoard(struct Hashmap* board);
struct Move* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta);
void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore);

void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth) {
	int x = getX(board), y = getY(board);
	struct Move* move = minimax(board, x, y, *column - 1, centres, PLAYER_2_TOKEN, depth, depth, INT_MIN, INT_MAX);
	*column = move->column + 1;
	//printf("\ndepth %d: final score & column = %d, %d", settings->depth, move->score, move->column + 1);
	free(move);
}

struct Hashmap* copyBoard(struct Hashmap* board, int x, int y) {
	struct Hashmap* copy = createTable(x, y);
	for (int j = 0; j < x; j++) {
		for (int k = 0; k < y; k++) {
			int* tok = malloc(sizeof(int));
			*tok = *((int*)getToken(board, j, k));
			if (*tok != EMPTY_SLOT)
				addMove(copy, j, tok);
		}
	}
	return copy;
}

bool isGameOver(struct Hashmap* board, int row, int column) {
	//printf("\ncheckWin P1 = %s, P2 = %s >> row: %d, column: %d\n", checkWin(row, column, board, PLAYER_1_TOKEN) ? "true" : "false", checkWin(row, column, board, PLAYER_2_TOKEN) ? "true" : "false", row, column);
	return checkWin(row, column, board, PLAYER_1_TOKEN) || checkWin(row, column, board, PLAYER_2_TOKEN) || isBoardFull(board, getX(board));
}

struct Move* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta) {
	struct Move* move = (struct Move*)malloc(sizeof(struct Move));
	move->column = column;
	//printf("\n%d. (%d, %d), col: %d, tok: %d", depth, x, y, column, player);
	int row = hashGet(board, column)->top;
	move->gameOver = isGameOver(board, row, column);
	//printf(" >> gameOver? %s", move->gameOver ? "true" : "false");

	if (depth == 0 || move->gameOver) {
		if (move->gameOver) {
			if (checkWin(row, column, board, PLAYER_2_TOKEN)) { //bot has won in this instance
				/*The idea of this calculation is to give wins closer to the boards current state a
				* higher priority, as we would prefer the AI to move on those instead. We don't want
				* it to prioritise wins that are only possible further in the future.*/
				int score = (int)round(150 * maxDepth / (float)(maxDepth - depth));

				if (maxDepth > 3 && depth < maxDepth - 1) { //this is used to detect if the move to be made will give the player a win when we can't get one
					row++;
					bool pWin = false;
					int* tok = malloc(sizeof(int));
					*tok = PLAYER_1_TOKEN;
					if(!addMove(board, move->column, tok))
						pWin = checkWin(row, move->column, board, PLAYER_1_TOKEN);
					move->score = pWin ? (int)round(-150 * maxDepth / (float)(maxDepth - depth)) : score;
				}
				else
					move->score = score;
			}
			else if (checkWin(row, column, board, PLAYER_1_TOKEN)) //player has won in this instance
				move->score = (int)round(-150 * maxDepth / (float)(maxDepth - depth));
			else
				move->score = 0; //board is full
			//printf("\n>> gameOver, column = %d, row = %d, score = %d, tok = %d", column, row, move->score, player);
		}
		else
			getScore(board, centres, x, y, &(move)->score); //evaluate the state of the final instance
		//printf(" >> score%d = %d", column, move->score);
		return move;
	}
	if (player == PLAYER_2_TOKEN) { //maximizing player
		move->score = INT_MIN;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct Hashmap* temp = copyBoard(board, x, y);
				int* tok = malloc(sizeof(int));
				*tok = PLAYER_2_TOKEN;
				addMove(temp, i, tok);

				//for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
				//	printf("\n|");
				//	for (int k = 0; k < x; k++) {
				//		int p = getToken(temp, k, (y - 1) - j);
				//		printf("%d|", p);
				//	}
				//}
				//printf("d:%d i:%d", depth, i);

				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1, maxDepth, alpha, beta);

				if (newMove->score > move->score) {
					//printf("\nnewMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->gameOver = newMove->gameOver;
					move->column = move->gameOver && move->score < -1000 ? newMove->column : i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
				alpha = max(alpha, move->score);
				if (alpha >= beta)
					break;
			}
		}
		//printf("\nstep up");
		return move;
	}
	else { //minimizing player
		move->score = INT_MAX;
		for (int i = 0; i < x; i++) {
			if (!stackIsFull(hashGet(board, i))) {
				struct Hashmap* temp = copyBoard(board, x, y);
				int* tok = malloc(sizeof(int));
				*tok = PLAYER_1_TOKEN;
				addMove(temp, i, tok);

				//for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
				//	printf("\n|");
				//	for (int k = 0; k < x; k++) {
				//		int p = getToken(temp, k, (y - 1) - j);
				//		printf("%d|", p);
				//	}
				//}
				//printf("d:%d i:%d", depth, i);

				struct Move* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1, maxDepth, alpha, beta);

				if (newMove->score < move->score) {
					//printf("\nnewMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->gameOver = newMove->gameOver;
					move->column = move->gameOver ? newMove->column : i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeBoard(temp);
				free(newMove);
				beta = min(beta, move->score);
				if (alpha >= beta)
					break;
			}
		}
		//printf("\nstep up");
		return move;
	}
}

void evaluateWindow(int* window, int size, int* score) {
	//would there be less operations if we prevented this from running on empty and 1token:3empty windows?
	if (count(window, size, PLAYER_2_TOKEN) == 2 && count(window, size, EMPTY_SLOT) == 2)
		*score += 2;
	else if (count(window, size, PLAYER_2_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score += 5;
	else if (count(window, size, PLAYER_2_TOKEN) == 4)
		*score += 100; //if minimax is being used, this is useless, but if we set the minimax depth to 0 this will have to be used (so this is used when the depth is set to 1)
	/*else if (count(window, size, PLAYER_1_TOKEN) == 2 && count(window, size, EMPTY_SLOT) == 2)
		*score -= 1;*/
	else if (count(window, size, PLAYER_1_TOKEN) == 3 && count(window, size, EMPTY_SLOT) == 1)
		*score -= 5; //was previously at 4 (appeared to work better in some instances)
	else if (count(window, size, PLAYER_1_TOKEN) == 4)
		*score -= 100; //same rendundancy as the AI token detection above

	//printf("\nwindow: %d, %d, %d, %d >> size: %d >> P2 count: %d >> NULL count: %d >> score: %d", window[0], window[1], window[2], window[3], size, count(window, size, PLAYER_2_TOKEN), count(window, size, EMPTY_SLOT), *score);
}

void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore) { //determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	//centre score - exists because moves made here give the AI more options
	for (int i = 0; i < 2; i++) {
		if (centres[i] != 0) { //prevents the check of a second centre column if there is only 1
			int* col = malloc(sizeof(int) * y);

			for (int j = 0; j < y; j++)
				col[j] = *((int*)getToken(board, centres[i], j));

			//here we would ideally pass the ARRAY_LENGTH instead of y, but we cannot do this as the compiler won't know the array length after malloc,
			//it will only recognize a pointer, thus giving us the length of that instead
			score += count(col, y, PLAYER_2_TOKEN); //maybe change to count of empty slots? As this would help prioritise the centre only if it had a decent amount of free spaces
			
			free(col);
		}
	}
	
	//horizontal score
	for (int i = 0; i < y; i++) {
		int* row = malloc(sizeof(int) * x);
		
		for (int j = 0; j < x; j++)
			row[j] = *((int*)getToken(board, j, i));

		for (int j = 0; j < x - 3; j++) {
			int window[4] = { row[j], row[j + 1], row[j + 2], row[j + 3] };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}		
		free(row);
	}

	//vertical score
	for (int i = 0; i < x; i++) {
		int* col = malloc(sizeof(int) * y);

		for (int j = 0; j < y; j++)
			col[j] = *((int*)getToken(board, i, j));

		for (int j = 0; j < y - 3; j++) {
			int window[4] = { col[j], col[j + 1], col[j + 2], col[j + 3] };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
		free(col);
	}
	
	//bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { *((int*)getToken(board, j, i)), *((int*)getToken(board, j + 1, i + 1)), *((int*)getToken(board, j + 2, i + 2)), *((int*)getToken(board, j + 3, i + 3)) };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { *((int*)getToken(board, j + 3, i)), *((int*)getToken(board, j + 2, i + 1)), *((int*)getToken(board, j + 1, i + 2)), *((int*)getToken(board, j, i + 3)) };
			evaluateWindow(window, ARRAY_LENGTH(window), &score);
		}
	}

	*finalScore = score;
}

//ideally we would calculate the array length here, but this isn't possible
//as the compiler doesn't know what the pointer is pointing to, so it cannot
//define the length of the array at compile time
int count(int* list, int n, int tok) {
	int c = 0;
	for (int i = 0; i < n; i++)
		if (list[i] == tok)
			c++;
	return c;
}

void freeBoard(struct Hashmap* board) { //used to clear the board data from memory
	for (int i = 0; i < board->size; i++) {
		for (int j = 0; j < board->list[i]->stack->size; j++) {
			if (j <= board->list[i]->stack->top && board->list[i]->stack->list[j]->val != NULL)
				free(board->list[i]->stack->list[j]->val);
			free(board->list[i]->stack->list[j]);
		}
		free(board->list[i]->stack);
		free(board->list[i]);
	}
	free(board->list);
	free(board);
}