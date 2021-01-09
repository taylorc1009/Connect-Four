//credit - https://www.youtube.com/watch?v=MMLtza3CZFM&list=WL&index=310&t=863s

#include <math.h>
#include "structs/Hashmap.h"

struct AIMove {
	int column;
	int score;
	bool gameOver;
};

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

// -- NOTICE -- count previously had a paramater n which was the size of the array
//and we'd use it for 'i < n'. It was given by ARRAY_LENGTH, but I noticed that
//ARRAY_LENGTH was giving it a size of 1, was this true? Or was it actually
//giving 4? I only ask because the AI is now performing differently, it seems to
//be better from what I've tested but I haven't done enough testing, for example,
//the AI used to always prioritise centre columns on the first few moves, but, if
//I am correct, that would only happen because it couldn't find any better moves
//and now it does because it's counting the correct window sizes
int count(int* list, int tok) {
	int c = 0;
	for (int i = 0; i < (int)sizeof(list); i++)
		if (list[i] == tok)
			c++;
	return c;
}

void evaluateWindow(int* window, int* score) {
	if (count(window, PLAYER_2_TOKEN) == 2 && count(window, EMPTY_SLOT) == 2)
		*score += 2;
	else if (count(window, PLAYER_2_TOKEN) == 3 && count(window, EMPTY_SLOT) == 1)
		*score += 5;
	else if (count(window, PLAYER_2_TOKEN) == 4)
		*score += 100; //if minimax is being used, this is useless, but if we set the minimax depth to 0 this will have to be used (so this is used when the depth is set to 1)
	else if (count(window, PLAYER_1_TOKEN) == 2 && count(window, EMPTY_SLOT) == 2)
		*score -= 1; //was previously commented out (does the AI have a better play style without this?)
	else if (count(window, PLAYER_1_TOKEN) == 3 && count(window, EMPTY_SLOT) == 1)
		*score -= 5; //was previously at 4 (appeared to work better in some instances)
	else if (count(window, PLAYER_1_TOKEN) == 4)
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

			score += count(col, EMPTY_SLOT); //only prioritises the centre column(s) depending on the amount of free spaces (it previously prioritised it if the AI had more tokens there, also 'y' used to by used instead of 'ARRAY_LENGTH')

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
			evaluateWindow(window, &score);
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
			evaluateWindow(window, &score);
		}
		free(col);
	}

	//bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { *((int*)getToken(board, j, i)), *((int*)getToken(board, j + 1, i + 1)), *((int*)getToken(board, j + 2, i + 2)), *((int*)getToken(board, j + 3, i + 3)) };
			evaluateWindow(window, &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			int window[4] = { *((int*)getToken(board, j + 3, i)), *((int*)getToken(board, j + 2, i + 1)), *((int*)getToken(board, j + 1, i + 2)), *((int*)getToken(board, j, i + 3)) };
			evaluateWindow(window, &score);
		}
	}

	*finalScore = score;
}

bool isGameOver(struct Hashmap* board, int row, int column) {
	//printf("\ncheckWin P1 = %s, P2 = %s >> row: %d, column: %d\n", checkWin(row, column, board, PLAYER_1_TOKEN) ? "true" : "false", checkWin(row, column, board, PLAYER_2_TOKEN) ? "true" : "false", row, column);
	return checkWin(row, column, board, PLAYER_1_TOKEN) || checkWin(row, column, board, PLAYER_2_TOKEN) || isBoardFull(board, getX(board));
}

struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta) {

	//you could do an 'if(column != NULL)' here? to try and stop crashing when the previous column changed was incorrect - this would prevent all the column tracking required in undo/redo and save/load
	struct AIMove* move = (struct AIMove*)malloc(sizeof(struct AIMove));
	move->column = column;
	//printf("\n%d. (%d, %d), col: %d, tok: %d", depth, x, y, column, player);
	int row = hashGet(board, column)->top;
	move->gameOver = isGameOver(board, row, column);
	//printf(" >> gameOver? %s", move->gameOver ? "true" : "false");

	if (depth == 0 || move->gameOver) {
		if (move->gameOver) {
			if (checkWin(row, column, board, PLAYER_2_TOKEN)) { //bot has won in this instance

				/*the idea of this calculation is to give wins closer to the boards current state a
				* higher priority as we would prefer the AI moved on those instead: we don't want
				* it to prioritise wins that are only possible further in the future*/
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

				struct AIMove* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1, maxDepth, alpha, beta);

				if (newMove->score > move->score) {
					//printf("\nnewMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->gameOver = newMove->gameOver;
					move->column = move->gameOver && move->score < -1000 ? newMove->column : i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeHashmap(temp);
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

				struct AIMove* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1, maxDepth, alpha, beta);

				if (newMove->score < move->score) {
					//printf("\nnewMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
					move->score = newMove->score;
					move->gameOver = newMove->gameOver;
					move->column = move->gameOver ? newMove->column : i;
					//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
				}
				freeHashmap(temp);
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

void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth) {
	int x = getX(board), y = getY(board);
	struct AIMove* move = minimax(board, x, y, *column - 1, centres, PLAYER_2_TOKEN, depth, depth, INT_MIN, INT_MAX);
	*column = move->column + 1;
	//printf("\ndepth %d: final score & column = %d, %d", settings->depth, move->score, move->column + 1);
	free(move);
}