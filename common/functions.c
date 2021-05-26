#include "functions.h"

void delay(int numOfSeconds) {
	#if !_WIN32
	unsigned int retTime = time(0) + numOfSeconds;
    while (time(0) < retTime); // Unix 'sleep' - the block below does not work with Unix's standard library
	#else
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
	#endif
}

void cleanStdin() {
	char c = NUL;
	while ((c = getchar()) != '\n' && c != 0) { /* do nothing until input buffer is fully flushed */ }
}

int validateOption(int min, int max, bool inPlay) { //used to validate integers within a given range - because we now use 'fgets' instead of 'scanf', I need to find a way to get numbers with more than 1 digit safely
	bool valid;
	int num;
	char buffer[3]; //I'm aware that this limits the input to numbers with 1 digit, but we'll never be using any > 9 anyway

	do {
		fgets(buffer, sizeof buffer, stdin);

		num = (int)buffer[0];
		const bool juncture = buffer[1] == '\n';

		valid = (((num >= min + '0' && num <= max + '0') || (inPlay && (num == 'r' || num == 'u' || num == 's'))) && juncture);
		if (!valid) {
			if (!juncture)
				cleanStdin();
			if (min == 0 && max == 0)
				printf("\n(!) invalid input: please enter an undo/redo operation or 0 to cancel\n> "); //used during the AI move being held as only 0 can be entered
			else
				printf("\n(!) invalid input: please re-enter an number between %d and %d\n> ", min, max);
		}
	} while (!valid);

	return num - '0';
}

struct Matrix* checkWin(int row, int column, struct Hashmap* board, int token) {
	int x = getX(board), y = getY(board);

	//horizontal check
	int count = 0;
	for (int i = (column - 3 < 0 ? 0 : column - 3); i < (column + 4 > x ? x : column + 4); i++) {
		if (*((int*)getToken(board, i, row)) == token) {
			count++;
			if (count >= 4) {
				struct Matrix* win = createMatrix(4, 2);
				for (int j = 0; j < 4; j++) {
					*((int*)matrixCell(win, 0, j)) = (i - 3) + j;
					*((int*)matrixCell(win, 1, j)) = row;
				}
				return win;
			}
		}
		else
			count = 0;
	}

	//vertical check
	count = 0;
	for (int i = (row - 3 < 0 ? 0 : row - 3); i < (row + 4 > y ? y : row + 4); i++) {
		if (*((int*)getToken(board, column, i)) == token) {
			count++;
			if (count >= 4) {
				struct Matrix* win = createMatrix(4, 2);
				for (int j = 0; j < 4; j++) {
					*((int*)matrixCell(win, 0, j)) = column;
					*((int*)matrixCell(win, 1, j)) = i - j;
				}
				return win;
			}
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
			if (count >= 4) {
				struct Matrix* win = createMatrix(4, 2);
				for (int k = 0; k < 4; k++) {
					*((int*)matrixCell(win, 0, k)) = j - k;
					*((int*)matrixCell(win, 1, k)) = i - k;
				}
				return win;
			}
		}
		else
			count = 0;
	}

	//bottom-right to top-left diagonal check
	count = 0;
	i = row;
	j = column;

	//creates check point to start checking from diagonally
	while (i != 0 && j != x - 1 && i > row - 3 && j < column + 3) {
		i--;
		j++;
	}

	for (i, j; i < y && j >= 0 && i < row + 4 && j > column - 4; i++, j--) {
		if (*((int*)getToken(board, j, i)) == token) {
			count++;
			if (count >= 4) {
				struct Matrix* win = createMatrix(4, 2);
				for (int k = 0; k < 4; k++) {
					*((int*)matrixCell(win, 0, k)) = j + k;
					*((int*)matrixCell(win, 1, k)) = i - k;
				}
				return win;
			}
		}
		else
			count = 0;
	}

	return NULL;
}