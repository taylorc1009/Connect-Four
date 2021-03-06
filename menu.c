/* TODO
*	Implement:
*	- Clean up the code (some things may still have their first, inefficient implementation, but remain as they always have simply because they work)
*		- Also, use Code Inspector or CMake to check for security/standards violations
*	- Finish internal commentary
*	- Randomize (or allow selection) of player or AI turn on first move?
*	- Add a move down animation?
*
*	Update README.md upon completion (if required)
*/

#include "menu.h"

int removeExcessSpaces(char* str) { //used to remove preceding and exceding spaces from strings
	int i, j;

	for (i = j = 0; str[i]; ++i)
		if (str[i] != '\n' && !isspace(str[i]) || (i > 0 && isspace(str[i]) && !isspace(str[i - 1])))
			str[j++] = str[i];

	if (j > 0 && isspace(str[j - 1])) {
		str[j - 1] = '\0';
		j--;
	}

	return j; //return the new length of the string
}

char* inputString(FILE* fp, size_t size) { //credit - https://stackoverflow.com/a/16871702/11136104
	char* str = realloc(NULL, sizeof(*str) * size);
	if (!str)
		return str;
	
	int ch;
	size_t len = 0;

	while (EOF != (ch = fgetc(fp)) && ch != '\n') {
		str[len++] = ch;
		if (len == size) {
			str = realloc(str, sizeof(*str) * (size += 16));

			if (!str)
				return str;
		}
	}
	str[len++] = '\0';

	return realloc(str, sizeof(*str) * len);
}

void getPlayerName(char** name, int* nameSize, int playerNum) {
	bool empty;

	printf("\n%sPlayer %d%s, please enter your name\n> ", playerNum == 1 ? PLAYER_1_COLOUR : PLAYER_2_COLOUR, playerNum, DEFAULT_COLOUR);

	do {
		*name = inputString(stdin, 30);
		empty = !*name || *name[0] == '\0';

		if (empty)
			printf("\n(!) name empty, please enter one\n> ");
		else
			*nameSize = removeExcessSpaces(*name);
	} while (empty);
}

void setup(struct Settings* settings) {
	getPlayerName(&settings->player1, &settings->player1Size, 1);

	if (settings->solo) {
		settings->player2 = "AI"; //this makes the char* static so no point dynamically allocating before
		settings->player2Size = 3;

		printf("\nWelcome %s%s%s! Which difficulty level would you like to play at?\n\n 1 - easy\n 2 - medium\n 3 - hard\n 4 - expert\n\n> ", PLAYER_1_COLOUR, settings->player1, DEFAULT_COLOUR);
		switch (validateOption(1, 4, false)) {
			case 1:
				settings->depth = 1;
				break;
			case 2:
				settings->depth = 3;
				break;
			case 3:
				settings->depth = 5;
				break;
			case 4:
				settings->depth = 7;
				break;
		}
	}
	else {
		getPlayerName(&settings->player2, &settings->player2Size, 2);
		printf("\nWelcome %s%s%s and %s%s%s!", PLAYER_1_COLOUR, settings->player1, DEFAULT_COLOUR, PLAYER_2_COLOUR, settings->player2, DEFAULT_COLOUR);
		delay(1);
	}

	printf("\nStarting...");
	delay(2);
	play(NULL, NULL, settings, true, false);

	free(settings->player1);
	if (!settings->solo)
		free(settings->player2);
}

void welcome(int x, int y) {
	system(CLEAR_TERMINAL);
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney\nTo continue, select either:\n\n 1 - how to play + controls\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - load a previous save\n 6 - quit\n", x, y);
}

int main(int argc, char** argv) {
	#if !__WIN32
	//remove this and you'll notice that 'printf' outputs are buffered (they happen after delays, terminal clears...), but it only happens on Unix by default
	//reference - https://stackoverflow.com/a/16877432
	setbuf(stdout, NULL);
	#endif

	struct Settings* settings = (struct Settings*)malloc(sizeof(struct Settings));
	settings->boardX = 7;
	settings->boardY = 6;

	welcome(settings->boardX, settings->boardY);

	int option;
	do {
		settings->solo = false;
		settings->depth = 0;

		printf("\nWhat would you like to do?\n> ");
		option = validateOption(1, 6, false);
		switch (option) {
		case 1:
			printf("\nConnect 4 is a rather simple game. Both players take a turn each selecting a column\nwhich they would like to drop their token (player 1 = %sRED%s, player 2 = %sYELLOW%s) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally. Here are the in-game controls:\n\n 1-9 = column you wish to place your token in\n 0 = exit\n u = undo\n r = redo \n s = save\n", PLAYER_1_COLOUR, DEFAULT_COLOUR, PLAYER_2_COLOUR, DEFAULT_COLOUR);
			break;

		case 2:
			printf("\nPlease enter the width (amount of columns) you want to play with (5-9)\n> ");
			settings->boardX = validateOption(5, 9, false);

			printf("\nPlease enter the height (amount of rows) you want to play with (5-9)\n> ");
			settings->boardY = validateOption(5, 9, false);

			printf("\nBoard dimensions changed successfully to %dx%d\n", settings->boardX, settings->boardY);
			delay(2);

			welcome(settings->boardX, settings->boardY);
			break;

		case 3:
			setup(settings);

			welcome(settings->boardX, settings->boardY);
			break;

		case 4:
			settings->solo = true;
			setup(settings);

			welcome(settings->boardX, settings->boardY);
			break;

		case 5: ; //reason for ';' - https://stackoverflow.com/questions/18496282/why-do-i-get-a-label-can-only-be-part-of-a-statement-and-a-declaration-is-not-a
			struct Hashmap* board = NULL;
			struct Hashmap* history = NULL;
			bool turn, traversing;

			char* response = (char*)loadGame(&board, &history, settings, &turn, &traversing);

			if (response == NULL) {
				printf("\nGame loaded!");
				delay(1);
				printf("\nStarting...");
				delay(2);

				play(&board, &history, settings, turn, traversing);

				welcome(settings->boardX, settings->boardY);
			}
			else
				printf("%s", response);
				//delay(2); //give some time to show the error message

			break;

		case 6:
			free(settings); //player names are deallocated when the game is over/quit

			system(CLEAR_TERMINAL);
			printf("Connect 4 closed, goodbye!\n");
			break;
		}
	} while (option != 6);

	return 0;
}