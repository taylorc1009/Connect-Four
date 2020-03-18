#include <stdio.h>
#include <string.h>

typedef enum { false, true } bool;

struct Settings {
	int boardX;
	int boardY;
	char *player1;
	char *player2;
};

void setupDual(struct Settings *settings);
void setupSolo(struct Settings *settings);

int main(int argc, char **argv) {
	system("cls");

	struct Settings *settings = malloc(sizeof(struct Settings));
	settings->boardX = 9;
	settings->boardY = 7;
	settings->player1 = malloc(sizeof(char) * 10);
	settings->player2 = malloc(sizeof(char) * 10);

	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit", settings->boardX, settings->boardY);
	
	mainmenu:
		printf("\n\nWhat would you like to do?\n> ");
		int option = validateOption(1, 5);
		switch (option) {
			case 1:
				printf("\nWelcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit\n\nConnect 4 is a rather simple game. Both users take a turn each selecting a column\nwhich they would like to drop their token (player 1 = O, player 2 = X) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally.", settings->boardX, settings->boardY);
				goto mainmenu;

			case 2:
				printf("\nPlease enter the width (amount of columns) you want to play with\n> ");
				settings->boardX = validateOption(3, 12);
				printf("\nPlease enter the height (amount of rows) you want to play with\n> ");
				settings->boardY = validateOption(3, 12);
				printf("\nBoard dimensions changed successfully to %dx%d", settings->boardX, settings->boardY);
				goto mainmenu;

			case 3:
				setupDual(settings);
				printf("%s, %s", settings->player1, settings->player2);
				goto mainmenu;

			case 4:
				setupSolo(settings);
				goto mainmenu;
			case 5:
				break;
		}

	system("cls");
	printf("Connect 4 closed, goodbye!\n");
	return 0;
}

int validateOption(int min, int max) {
	bool valid = false;
	int num = 0;

	while (!valid) {
		char term;
		if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= min && num <= max)) {
			printf("\n! invalid input: please re-enter an number between %d and %d\n> ", min, max);
			fflush(stdin); // maybe move before 'if'
		}
		else
			valid = true;
	}
	return num;
}

void setupDual(struct Settings *settings) {

}

void setupSolo(struct Settings *settings) {
	
}