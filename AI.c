#include <math.h>
#include "AI.h"

struct Hashmap* copyBoard(const struct Hashmap* restrict board, const int x, const int y) {
	struct Hashmap* copy = createTable(x, y);
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y; j++) {
			int temp = *((int*)getToken(board, i, j));
			if (temp != EMPTY_SLOT) {
				int* token = malloc(sizeof(int));
				*token = temp;
				addMove(copy, i, token);
			}
			else
				break;
		}
	}
	return copy;
}

int count(const int* restrict window, const int token) {
	int c = 0;
	for (int i = 0; i < 4; i++) //the window size will always be 4
		if (window[i] == token)
			c++;
	return c;
}

void evaluateWindow(const int* restrict window, int* restrict score) {
	const int countP1 = count(window, PLAYER_1_TOKEN), countP2 = count(window, PLAYER_2_TOKEN), countEmpty = count(window, EMPTY_SLOT);

	// -- NOTICE -- I tried adding/subtracting 50 when the count was 4, which seemed to give good results but whether they were better or not will need to be tested
	if (countP2 == 2 && countEmpty == 2)
		*score += 2;
	else if (countP2 == 3 && countEmpty == 1)
		*score += 5;
	else if (countP2 == 4)
		*score += 50; //if Minimax is being used, this is useless, but if we set the Minimax depth to 0 this will have to be used (so this is used when the depth is set to 1)
	else if (countP1 == 2 && countEmpty == 2)
		*score -= 2; //was previously commented out (does the AI have a better play style without this?)
	else if (countP1 == 3 && countEmpty == 1)
		*score -= 5; //was previously at 4 (appeared to work better in some instances)
	else if (countP1 == 4)
		*score -= 50; //same redundancy as the AI token detection above

	//printf("\nwindow: %d, %d, %d, %d >> P2 count: %d >> NULL count: %d >> score: %d", window[0], window[1], window[2], window[3], count(window, PLAYER_2_TOKEN), count(window, EMPTY_SLOT), *score);
}

void getScore(const struct Hashmap* restrict board, const int* restrict centres, const int x, const int y, int* restrict finalScore) { //determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	//centre score - exists because moves made here give the AI more options
	for (int i = 0; i < 2; i++)
		if (centres[i] != 0) //prevents the check of a second centre column if there is only 1
			for (int j = 0; j < y; j++)
				if (*((int*)getToken(board, centres[i], j)) == EMPTY_SLOT)
					score++; //only prioritises the centre column(s) depending on the amount of free spaces (it previously prioritised it if the AI had more tokens there)

	//horizontal score
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x - 3; j++) {
			const int window[4] = {
				*((int*)getToken(board, j, i)),
				*((int*)getToken(board, j + 1, i)),
				*((int*)getToken(board, j + 2, i)),
				*((int*)getToken(board, j + 3, i))
			};
			evaluateWindow(window, &score);
		}
	}

	//vertical score
	for (int i = 0; i < x; i++) {
		for (int j = 0; j < y - 3; j++) {
			const int window[4] = {
				*((int*)getToken(board, i, j)),
				*((int*)getToken(board, i, j + 1)),
				*((int*)getToken(board, i, j + 2)),
				*((int*)getToken(board, i, j + 3))
			};
			evaluateWindow(window, &score);
		}
	}

	//bottom-right to top-left diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			const int window[4] = {
				*((int*)getToken(board, j, i)),
				*((int*)getToken(board, j + 1, i + 1)),
				*((int*)getToken(board, j + 2, i + 2)),
				*((int*)getToken(board, j + 3, i + 3))
			};
			evaluateWindow(window, &score);
		}
	}

	//bottom-left to top-right diagonal score
	for (int i = 0; i < y - 3; i++) {
		for (int j = 0; j < x - 3; j++) {
			const int window[4] = {
				*((int*)getToken(board, j + 3, i)),
				*((int*)getToken(board, j + 2, i + 1)),
				*((int*)getToken(board, j + 1, i + 2)),
				*((int*)getToken(board, j, i + 3))
			};
			evaluateWindow(window, &score);
		}
	}

	*finalScore = score;
}

GameStatus isGameOver(const struct Hashmap* restrict board, const int row, const int column) {
	//printf("\ncheckWin P1 = %d, P2 = %d >> row: %d, column: %d\n", checkWin(row, column, board, PLAYER_1_TOKEN), checkWin(row, column, board, PLAYER_2_TOKEN), row, column);
	if (checkWin(row, column, board, PLAYER_2_TOKEN))
		return AIWin;
	if (checkWin(row, column, board, PLAYER_1_TOKEN))
		return PlayerWin;
	if (isBoardFull(board, getX(board)))
		return BoardFull;
	return Ongoing;
}

int safeWinScore(int scale, int depth, int maxDepth) {
	/* the idea of this calculation is to give wins closer to the boards current state a
	 * higher priority as we would prefer the AI moved on those instead: we don't want
	 * it to prioritise wins that are only possible further in the future*/
	return (int)round((150 * (int)scale * maxDepth) / (float)(maxDepth - depth));
}

struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta) {
	struct AIMove* move = (struct AIMove*)malloc(sizeof(struct AIMove));
	int row = 0;
	if (column != -1) {
		move->column = column;
		row = hashGet(board, column)->top;
		//printf("\n%d. (%d, %d), row: %d col: %d, token: %d", depth, x, y, row, column, player);
		move->gameStatus = isGameOver(board, row, column);
		//printf(" >> gameStatus? %d", move->gameStatus);
	}
	else
		move->gameStatus = Ongoing;

	if (depth == 0 || move->gameStatus) {
		if (move->gameStatus) {
			if (move->gameStatus == AIWin) { //AI has won in this instance
				int score;

				if (maxDepth > 3 && depth < maxDepth - 1) { //discourages the AI from making future moves that give the player a win ('> 3' so it only does this for high difficulties and dept < max so it will not prevent a win on the next move)
					row++;

					bool pWin = false;
					int* token = malloc(sizeof(int));
					*token = PLAYER_1_TOKEN;
					if (addMove(board, move->column, token)) //if it is unsuccessful as the column is full, that doesn't matter as we're only trying to prevent the player winning by placing a token on top of the AI's
						pWin = checkWin(row, move->column, board, PLAYER_1_TOKEN) != NULL ? true : false;
					else
						free(token);
					score = pWin ? safeWinScore(-1, depth, maxDepth) : safeWinScore(1, depth, maxDepth);
				}
				else
					score = safeWinScore(1, depth, maxDepth);

				move->score = score;
			}
			else if (move->gameStatus == PlayerWin) //player has won in this instance
				move->score = safeWinScore(-1, depth, maxDepth);
			else //move->gameStatus == BoardFull
				move->score = 0;
			//printf("\n>> column = %d, row = %d, score = %d, token = %d", column, row, move->score, player);
		}
		else
			getScore(board, centres, x, y, &(move)->score); //evaluate the state of the final instance
		//printf(" >> score%d = %d", column, move->score);
		return move;
	}
	if (player == PLAYER_2_TOKEN) { //maximizing player
		move->score = INT_MIN;
		for (int i = 0; i < x; i++) {
			struct Hashmap* temp = copyBoard(board, x, y);
			int* token = malloc(sizeof(int));
			*token = PLAYER_2_TOKEN;
			struct AIMove* newMove;

			if (addMove(temp, i, token)) //I previously used the 'columnIsFull' detection above, but when the board state came to a point where the AI could only make bad moves, it would choose the full column as it's score was still 0 (therefore greater than a negative number)
				newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1, maxDepth, alpha, beta);
			else {
				free(token);
				newMove = malloc(sizeof(struct AIMove));
				
				newMove->score = INT_MIN;
				newMove->gameStatus = Ongoing;
				newMove->column = i;
			}

			/*for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
				printf("\n|");
				for (int k = 0; k < x; k++) {
					int p = *((int*)getToken(temp, k, (y - 1) - j));
					printf("%d|", p);
				}
			}
			printf("d:%d i:%d\n", depth, i);*/

			if (newMove->score > move->score) {
				//printf("\nmaximizing-newMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
				move->score = newMove->score;
				move->gameStatus = newMove->gameStatus;
				move->column = move->gameStatus && move->score < -1000 ? newMove->column : i; //prevents the maximizing player from using a really low score (we still return the other values so we can let the algorithm know what happens)
			}
			freeHashmap(temp);
			free(newMove);

			alpha = MAX(alpha, move->score);
			if (alpha >= beta)
				break;
		}
		//printf("\nstep up");
		return move;
	}
	else { //minimizing player
		move->score = INT_MAX;
		for (int i = 0; i < x; i++) {
			struct Hashmap* temp = copyBoard(board, x, y);
			int* token = malloc(sizeof(int));
			*token = PLAYER_1_TOKEN;
			struct AIMove* newMove;

			if (addMove(temp, i, token)) //same as above, but for the minimizing player
				newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1, maxDepth, alpha, beta);
			else {
				free(token);
				newMove = malloc(sizeof(struct AIMove));

				newMove->score = INT_MAX;
				newMove->gameStatus = Ongoing;
				newMove->column = i;
			}

			/*for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
				printf("\n|");
				for (int k = 0; k < x; k++) {
					int p = *((int*)getToken(temp, k, (y - 1) - j));
					printf("%d|", p);
				}
			}
			printf("d:%d i:%d\n", depth, i);*/

			if (newMove->score < move->score) {
				//printf("\nminimizing-newMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
				move->score = newMove->score;
				move->gameStatus = newMove->gameStatus;
				move->column = move->gameStatus && move->score > 1000 ? newMove->column : i; //prevents the minimizing player from using a really high (same as the one above; setting these to lower values may make the AI smarter, but too low may break things)
			}
			freeHashmap(temp);
			free(newMove);

			beta = MIN(beta, move->score);
			if (alpha >= beta)
				break;
		}
		//printf("\nstep up");
		return move;
	}
}

void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth) {
	struct AIMove* move = minimax(board, getX(board), getY(board), -1, centres, PLAYER_2_TOKEN, depth, depth, INT_MIN, INT_MAX);
	*column = move->column + 1;
	//printf("\ndepth %d: final score & column = %d, %d", depth, move->score, move->column + 1);
	//delay(10);
	free(move);
}