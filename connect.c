#include <stdio.h>

typedef enum { false, true } bool;

int main(int argc, char **argv)
{
	system("cls");
	int boardX = 9;
	int boardY = 7;
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit", boardX, boardY);
	mainmenu:
		printf("\n\nWhat would you like to do?\n> ");
		int option = validateOption(1, 5);
		switch (option) {
			case 1:
				printf("\nConnect 4 is a rather simple game. Both users take a turn each selecting a column\nwhich they would like to drop their token (player 1 = O, player 2 = X) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally.");
				goto mainmenu;
			case 2:
				printf("");
				goto mainmenu;
			case 3:
				printf("");
				goto mainmenu;
			case 4:
				printf("");
				goto mainmenu;
			case 5:
				return 0;
		}
	system("cls");
	return 0;
}

int validateOption(int min, int max) {
	bool valid = false;
	int num = 0;
	while (!valid) {
		char term;
		if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= min && num <= max)) {
			printf("\n! invalid input: please re-enter\n> ");
			fflush(stdin);
		}
		else
			valid = true;
	}
	return num;
}