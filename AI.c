//credit - https://www.youtube.com/watch?v=MMLtza3CZFM&list=WL&index=310&t=863s

#include <math.h>
#include "structs/Hashmap.h"

typedef enum {
	Ongoing = 0,
	AIWin = 1,
	PlayerWin = 2,
	BoardFull = 3
} GameStatus;

struct AIMove {
	int column;
	int score;
	GameStatus gameStatus; //0 = game is still going, 1 = AI wins, 2 = player wins, 3 = board is full
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

int count(int* list, int tok) {
	int c = 0;
	for (int i = 0; i < 4; i++) //the window size will always be 4
		if (list[i] == tok)
			c++;
	return c;
}

void evaluateWindow(int* window, int* score) {
	// -- NOTICE -- I tried adding/subtracting 50 when the count was 4, which seemed to give good results but whether they were better or not will need to be tested
	if (count(window, PLAYER_2_TOKEN) == 2 && count(window, EMPTY_SLOT) == 2)
		*score += 2;
	else if (count(window, PLAYER_2_TOKEN) == 3 && count(window, EMPTY_SLOT) == 1)
		*score += 5;
	else if (count(window, PLAYER_2_TOKEN) == 4)
		*score += 50; //if minimax is being used, this is useless, but if we set the minimax depth to 0 this will have to be used (so this is used when the depth is set to 1)
	else if (count(window, PLAYER_1_TOKEN) == 2 && count(window, EMPTY_SLOT) == 2)
		*score -= 2; //was previously commented out (does the AI have a better play style without this?)
	else if (count(window, PLAYER_1_TOKEN) == 3 && count(window, EMPTY_SLOT) == 1)
		*score -= 5; //was previously at 4 (appeared to work better in some instances)
	else if (count(window, PLAYER_1_TOKEN) == 4)
		*score -= 50; //same rendundancy as the AI token detection above

	//printf("\nwindow: %d, %d, %d, %d >> P2 count: %d >> NULL count: %d >> score: %d", window[0], window[1], window[2], window[3], count(window, PLAYER_2_TOKEN), count(window, EMPTY_SLOT), *score);
}

void getScore(struct Hashmap* board, int* centres, int x, int y, int* finalScore) { //determines the best column to make a play in by giving each a score based on their current state
	int score = 0;

	//centre score - exists because moves made here give the AI more options
	for (int i = 0; i < 2; i++)
		if (centres[i] != 0) //prevents the check of a second centre column if there is only 1
			for (int j = 0; j < y; j++)
				if (*((int*)getToken(board, centres[i], j)) == EMPTY_SLOT)
					score++; //only prioritises the centre column(s) depending on the amount of free spaces (it previously prioritised it if the AI had more tokens there)

	//horizontal score
	int* row = malloc(sizeof(int) * x);
	for (int i = 0; i < y; i++) {
		for (int j = 0; j < x; j++)
			row[j] = *((int*)getToken(board, j, i));

		for (int j = 0; j < x - 3; j++) {
			int window[4] = { row[j], row[j + 1], row[j + 2], row[j + 3] };
			evaluateWindow(window, &score);
		}
	}
	free(row);

	//vertical score
	int* col = malloc(sizeof(int) * y);
	for (int i = 0; i < x; i++) {
		
		for (int j = 0; j < y; j++)
			col[j] = *((int*)getToken(board, i, j));

		for (int j = 0; j < y - 3; j++) {
			int window[4] = { col[j], col[j + 1], col[j + 2], col[j + 3] };
			evaluateWindow(window, &score);
		}
	}
	free(col);

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

enum GameStatus isGameOver(struct Hashmap* board, int row, int column) {
	//printf("\ncheckWin P1 = %d, P2 = %d >> row: %d, column: %d\n", checkWin(row, column, board, PLAYER_1_TOKEN), checkWin(row, column, board, PLAYER_2_TOKEN), row, column);
	if (checkWin(row, column, board, PLAYER_2_TOKEN))
		return AIWin;
	else if (checkWin(row, column, board, PLAYER_1_TOKEN))
		return PlayerWin;
	else if (isBoardFull(board, getX(board)))
		return BoardFull;
	return Ongoing;
}

int safeWinScore(int scale, int depth, int maxDepth) {
	/*the idea of this calculation is to give wins closer to the boards current state a
	 * higher priority as we would prefer the AI moved on those instead: we don't want
	 * it to prioritise wins that are only possible further in the future*/
	return (int)round((150 * (int)scale * maxDepth) / (float)(maxDepth - depth));
}

struct AIMove* minimax(struct Hashmap* board, int x, int y, int column, int* centres, int player, int depth, int maxDepth, int alpha, int beta) {

	//you could do an 'if(column != NULL)' here? to try and stop crashing when the previous column changed was incorrect - this would prevent all the column tracking required in undo/redo and save/load
	struct AIMove* move = (struct AIMove*)malloc(sizeof(struct AIMove));
	move->column = column;
	int row = hashGet(board, column)->top;
	//printf("\n%d. (%d, %d), row: %d col: %d, tok: %d", depth, x, y, row, column, player);
	move->gameStatus = isGameOver(board, row, column);
	//printf(" >> gameStatus? %s", move->gameStatus ? "true" : "false");

	if (depth == 0 || move->gameStatus) {
		if (move->gameStatus) {
			if (move->gameStatus == AIWin) { //bot has won in this instance
				int score;

				if (maxDepth > 3 && depth < maxDepth - 1) { //discourages the AI from making future moves that give the player a win ('> 3' so it only does this for high difficulties and dept < max so it will not prevent a win on the next move)
					row++;

					bool pWin = false;
					int* tok = malloc(sizeof(int));
					*tok = PLAYER_1_TOKEN;
					if (!addMove(board, move->column, tok)) //if it is unsuccessful as the column is full, that doesn't matter as we're only trying to prevent the player winning by placing a token on top of the AIs'
						pWin = checkWin(row, move->column, board, PLAYER_1_TOKEN);
					else
						free(tok);
					score = pWin ? safeWinScore(-1, depth, maxDepth) : safeWinScore(1, depth, maxDepth);;
				}
				else
					score = safeWinScore(1, depth, maxDepth);

				move->score = score;
			}
			else if (move->gameStatus == PlayerWin) //player has won in this instance
				move->score = safeWinScore(-1, depth, maxDepth);
			else //move->gameStatus == BoardFull
				move->score = 0;
			//printf("\n>> column = %d, row = %d, score = %d, tok = %d", column, row, move->score, player);
		}
		else
			getScore(board, centres, x, y, &(move)->score); //evaluate the state of the final instance
		//printf(" >> score%d = %d", column, move->score);
		return move;
	}
	if (player == PLAYER_2_TOKEN) { //maximizing player
		move->score = INT_MIN;
		for (int i = 0; i < x; i++) {
			if (columnIsFull(board, i))
				continue;

			struct Hashmap* temp = copyBoard(board, x, y);
			int* tok = malloc(sizeof(int));
			*tok = PLAYER_2_TOKEN;
			addMove(temp, i, tok);

			//for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
			//	printf("\n|");
			//	for (int k = 0; k < x; k++) {
			//		int p = *((int*)getToken(temp, k, (y - 1) - j));
			//		printf("%d|", p);
			//	}
			//}
			//printf("d:%d i:%d", depth, i);

			struct AIMove* newMove = minimax(temp, x, y, i, centres, PLAYER_1_TOKEN, depth - 1, maxDepth, alpha, beta);

			if (newMove->score > move->score) {
				//printf("\nnewMove = { %d, %d } > move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
				move->score = newMove->score;
				move->gameStatus = newMove->gameStatus;
				move->column = move->gameStatus && move->score < -1000 ? newMove->column : i; //prevents the maximizing player from using a really low score (we still return the other values so we can let the algorithm know what happens)
				//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
			}
			freeHashmap(temp);
			free(newMove);

			alpha = max(alpha, move->score);
			if (alpha >= beta)
				break;
		}
		//printf("\nstep up");
		return move;
	}
	else { //minimizing player
		move->score = INT_MAX;
		for (int i = 0; i < x; i++) {
			if (columnIsFull(board, i))
				continue;

			struct Hashmap* temp = copyBoard(board, x, y);
			int* tok = malloc(sizeof(int));
			*tok = PLAYER_1_TOKEN;
			addMove(temp, i, tok);

			//for (int j = 0; j < y; j++) { //displays the temporary board (for debugging)
			//	printf("\n|");
			//	for (int k = 0; k < x; k++) {
			//		int p = *((int*)getToken(temp, k, (y - 1) - j));
			//		printf("%d|", p);
			//	}
			//}
			//printf("d:%d i:%d", depth, i);

			struct AIMove* newMove = minimax(temp, x, y, i, centres, PLAYER_2_TOKEN, depth - 1, maxDepth, alpha, beta);

			if (newMove->score < move->score) {
				//printf("\nnewMove = { %d, %d } < move = { %d, %d }", newMove->score, newMove->column, move->score, move->column);
				move->score = newMove->score;
				move->gameStatus = newMove->gameStatus;
				move->column = move->gameStatus && move->score > 1000 ? newMove->column : i; //prevents the minimizing player from using a really high (same as the one above; setting these to lower values may make the AI smarter, but too low may break things)
				//printf(" >> move->score changed = %d, column = %d", move->score, move->column);
			}
			freeHashmap(temp);
			free(newMove);

			beta = min(beta, move->score);
			if (alpha >= beta)
				break;
		}
		//printf("\nstep up");
		return move;
	}
}

void AIMakeMove(struct Hashmap* board, int* column, int* centres, int depth) {
	int x = getX(board), y = getY(board);
	struct AIMove* move = minimax(board, x, y, *column - 1, centres, PLAYER_2_TOKEN, depth, depth, INT_MIN, INT_MAX);
	*column = move->column + 1;
	//printf("\ndepth %d: final score & column = %d, %d", depth, move->score, move->column + 1);
	//delay(10);
	free(move);
}