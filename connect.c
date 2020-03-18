#include <stdio.h>
#include <string.h>

#define NAME_MAX 32 // 2 extra to accomodate for '\0' and '\n'

typedef enum { false, true } bool;

struct Settings {
	int boardX;
	int boardY;
	char* player1;
	char* player2;
	bool solo;
};

void setup(struct Settings *settings);

void clean_stdin(void) { // causes app to hang
	int c;
	do {
		c = getchar();
	} while (c != '\n' && c != EOF);
}

int main(int argc, char **argv) {
	system("cls");

	struct Settings* settings = (struct Settings*) malloc(sizeof(struct Settings));
	settings->boardX = 9;
	settings->boardY = 7;
	settings->player1 = (char*)malloc(sizeof(char) * NAME_MAX);
	settings->player2 = (char*)malloc(sizeof(char) * NAME_MAX);

	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit", settings->boardX, settings->boardY);
	
	mainmenu:
		settings->solo = false;
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
				setup(settings);
				goto mainmenu;

			case 4:
				settings->solo = true;
				setup(settings);
				goto mainmenu;
			case 5:
				break;
		}
	exit:
		free(settings->player1);
		free(settings->player2);
		free(settings);
		system("cls");
		printf("Connect 4 closed, goodbye!\n");
		return 0;
}

int validateOption(int min, int max) {
	bool valid = false;
	int num;
	while (!valid) { // still causes an infinite loop if a char is entered
		char term;
		num = 0;
		if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= min && num <= max)) {
			printf("\n! invalid input: please re-enter an number between %d and %d\n> ", min, max);
			clean_stdin();
		} else
			valid = true;
	}
	return num;
}

void removeExcessSpaces(char* str) { // used to remove preceeding and exceeding spaces from strings
	int i, x;
	for (i = x = 0; str[i]; ++i)
		if (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1])))
			str[x++] = str[i];
	if (isspace(str[x - 1]))
		str[x - 1] = '\0';
	else
		str[x] = '\0';
}

void getName(char** player) { // dynamically resizes the allocation of the player name char array based on the input
	char buffer[NAME_MAX];
	getname:
		//clean_stdin();
		fgets(buffer, sizeof(buffer), stdin);
		removeExcessSpaces(buffer);
		size_t bufLen = strlen(buffer);
		if(buffer == 0 || buffer[0] == '\0') {
			printf("\n! name empty, please enter one\n> ");
			goto getname;
		} else if (bufLen > 30) {
			printf("\n! name to long, please re-enter\n> ");
			goto getname;
		} else {
			if (buffer[bufLen - 1] == '\n') {
				buffer[bufLen - 1] = '\0';
				bufLen--;
			}

			if (bufLen > sizeof(*player)) {
				*player = (char*)realloc(*player, sizeof(char) * bufLen);
			}
			strcpy(*player, buffer);
		}
}

void setup(struct Settings *settings) {
	printf("\nPlayer 1, please enter your name\n> ");
	getName(&(settings)->player1);

	if (settings->solo) {
		settings->player2 = "Bot";
		printf("\nWelcome %s!", settings->player1);
	}
	else {
		printf("\nPlayer 2, please enter your name\n> ");
		getName(&(settings)->player2);
		printf("\nWelcome %s and %s!", settings->player1, settings->player2);
	}
}