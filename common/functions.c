#include "functions.h"

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
}

void cleanStdin() {
	char c = NUL;
	while ((c = getchar()) != '\n' && c != 0) { /* do nothing until input buffer is fully flushed */ }
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
			if (min == 0 && max == 0)
				printf("\n(!) invalid input: please enter an undo/redo operation or 0 to cancel\n> "); //used during the AI move being held as only 0 can be entered
			else
				printf("\n(!) invalid input: please re-enter an number between %d and %d\n> ", min, max);
		}
	} while (!valid);

	return num - '0';
}

int** checkWin(int row, int column, struct Hashmap* board, int token) {
	int x = getX(board), y = getY(board);
	int* tok;

	//horizontal check
	int count = 0;
	for (int i = (column - 3 < 0 ? 0 : column - 3); i < (column + 4 > x ? x : column + 4); i++) {
		if (*((int*)getToken(board, i, row)) == token) {
			count++;
			if (count >= 4) {
				int** win = malloc(sizeof(int) * 4);
				for (int j = 0; j < 4; j++) {
					win[j] = malloc(sizeof(int) * 2);
					win[j][0] = (i - 3) + j;
					win[j][1] = row;
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
				int** win = malloc(sizeof(int) * 4);
				for (int j = 0; j < 4; j++) {
					win[j] = malloc(sizeof(int) * 2);
					win[j][0] = column;
					win[j][1] = i - j;
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
				int** win = malloc(sizeof(int) * 4);
				for (int k = 0; k < 4; k++) {
					win[k] = malloc(sizeof(int) * 2);
					win[k][0] = j - k;
					win[k][1] = i - k;
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
				int** win = malloc(sizeof(int) * 4);
				for (int k = 0; k < 4; k++) {
					win[k] = malloc(sizeof(int) * 2);
					win[k][0] = j + k;
					win[k][1] = i - k;
				}
				return win;
			}
		}
		else
			count = 0;
	}

	return NULL;
}