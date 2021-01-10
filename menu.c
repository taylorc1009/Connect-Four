/* TODO
*	Implement:
*	- Finish internal commentary
*	- Randomize (or allow selection) of player or AI turn on first move?
*	- Test save and load more
*	- Test undo and redo more
*	- Test how the 'evaluateWindow' 2player:2empty -=1 and count of EMPTY_SLOT in centre column(s) affects AI quality
*	- Look at if infinitely long player names are possible (using stdin and realloc)
*	- Add a move down animation?
*	
*	Update README.md upon completion (if required)
*/

#include <string.h>
#include <time.h>
#include "structs/Hashmap.h"
#include "game-structures.h"

#define NAME_MAX 32 //2 extra bytes to accomodate '\n' and '\0'

static inline void cleanStdin() {
	char c = NUL;
	while ((c = getchar()) != '\n' && c != 0) { /* do nothing until input buffer is fully flushed */ }
}

void delay(int numOfSeconds) {
	int milliSeconds = 1000 * numOfSeconds;
	clock_t startTime = clock();
	while (clock() < startTime + milliSeconds);
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

int removeExcessSpaces(char* str) { //used to remove preceding and exceding spaces from strings
	int i, j;

	for (i = j = 0; str[i]; ++i)
		if (str[i] != '\n' && (!isspace(str[i]) || (i > 0 && !isspace(str[i - 1]))))
			str[j++] = str[i];

	for (int k = j; k < i; k++)
		str[i] = 0;

	return j + 1; //return the new length of the string; +1 so we can add '\0' after
}

int getName(char** player) { //gets a player name and dynamically resizes the allocation of the char array based on the input
	char* buffer = (char*)malloc(sizeof(char) * NAME_MAX);
	int size;

	//comments here were an attempt to get an infinitely long string: one that exceeds the buffer size
	//char* input = (char*)malloc(sizeof(char));
	//int i = 0;
	//bool proceed = false;
	
	memset(buffer, NUL, NAME_MAX);
	//input = NUL;
	char* input = NULL;

	while (input == NULL) { /*&& proceed) {
		fgets(buffer, NAME_MAX, stdin);
		if (buffer == 0 || buffer[0] == '\n' || buffer[0] == '\0')
			printf("\n(!) name empty, please enter one\n> ");
		else {
			proceed = true;
			while (buffer[i] != '\n' && buffer[i] != 0 && i != NAME_MAX) {
				
				printf("%c", buffer[i]);
				int size = ARRAY_LENGTH(input);
				input = (char*)realloc(input, sizeof(char) * size);
				input[size - 1] = fgetc(stdin);
				i++;
			}
			if (i == NAME_MAX) {
				i = 0;
				proceed = (fseek(stdin, 0, SEEK_END), ftell(stdin)) > 0;
				rewind(stdin);
				if (proceed)
					fgets(buffer, NAME_MAX, stdin);
			}
		}*/
		input = fgets(buffer, NAME_MAX, stdin);

		//removeExcessSpaces(buffer);
		size_t bufLen = strlen(buffer);

		if (buffer == 0 || buffer[0] == '\n' || buffer[0] == '\0') {
			printf("\n(!) name empty, please enter one\n> ");
			input = NULL;
		} 
		else if (buffer[bufLen - 1] != '\n') {
			printf("\n(!) this name is too long (30 characters max), please re-enter\n> ");
			cleanStdin();
			input = NULL;
		}
		else {
			for (int i = 0; i < bufLen; i++) { //prevents ';' in player's names - having one in their name causes issues with save and load
				if (buffer[i] == ';') {
					printf("\n(!) the name entered contains a ';' - this is a system character, please re-enter\n> ");
					input = NULL;
				}
			}

			if (input == NULL)
				continue;

			/*if (input[bufLen - 1] == '\n') { removeExcessSpaces should do this
				input[bufLen - 1] = '\0';
				bufLen--;
			}*/
			size = removeExcessSpaces(input); //might be redundant due to removeExcessSpaces above

			if (bufLen > sizeof(*player))
				*player = (char*)realloc(*player, sizeof(char) * size);
			input[size - 1] = '\0';

			strcpy(*player, input);
		}
	}

	/*if (buffer[i] != '\n')
		cleanStdin();*/

	//removeExcessSpaces(input);
	//*player = (char*)realloc(*player, sizeof(char) * ARRAY_LENGTH(input));
	//strcpy(*player, input);
	free(buffer);
	//free(input);

	return size;
}

void setup(struct Settings* settings) {
	printf("\n%sPlayer 1%s, please enter your name\n> ", P1COL, PNRM);
	settings->player1 = (char*)malloc(sizeof(char) * NAME_MAX);
	settings->player1Size = getName(&(settings)->player1);

	if (settings->solo) {
		settings->player2 = "AI"; //this makes the char* static so no point dynamically allocating before
		settings->player2Size = 3;

		printf("\nWelcome %s%s%s! Which difficulty level would you like to play at?\n\n 1 - easy\n 2 - medium\n 3 - hard\n 4 - expert\n\n> ", P1COL, settings->player1, PNRM);
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
		printf("\n%sPlayer 2%s, please enter your name\n> ", P2COL, PNRM);
		settings->player2 = (char*)malloc(sizeof(char) * NAME_MAX);
		settings->player2Size = getName(&(settings)->player2);
		printf("\nWelcome %s%s%s and %s%s%s!", P1COL, settings->player1, PNRM, P2COL, settings->player2, PNRM);
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
	printf("Welcome to Connect 4! Reproduced virtually using C by Taylor Courtney\nTo continue, select either:\n\n 1 - how to play + controls\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - load a previous save\n 6 - quit\n", x, y);
}

int main(int argc, char** argv) {
	system("cls");

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
			printf("\nConnect 4 is a rather simple game. Both players take a turn each selecting a column\nwhich they would like to drop their token (player 1 = %sRED%s, player 2 = %sYELLOW%s) into next.\n\nThis continues until one player has connected 4 of their tokens in a row either\nhorizontally, vertically or diagonally. Here are the in-game controls:\n\n 1-9 = column you wish to place your token in\n 0 = exit\n u = undo\n r = redo \n s = save\n", P1COL, PNRM, P2COL, PNRM);
			break;

		case 2:
			printf("\nPlease enter the width (amount of columns) you want to play with (5-9)\n> ");
			settings->boardX = validateOption(5, 9, false);

			printf("\nPlease enter the height (amount of rows) you want to play with (5-9)\n> ");
			settings->boardY = validateOption(5, 9, false);

			printf("\nBoard dimensions changed successfully to %dx%d\n", settings->boardX, settings->boardY);
			delay(2);
			break;

		case 3:
			setup(settings);

			system("cls");
			welcome(settings->boardX, settings->boardY);
			break;

		case 4:
			settings->solo = true;
			setup(settings);

			system("cls");
			welcome(settings->boardX, settings->boardY);
			break;

		case 5:
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

				system("cls");
				welcome(settings->boardX, settings->boardY);
			}
			else
				printf("%s", response);
				//delay(2); //give some time to show the error message

			break;

		case 6:
			free(settings);

			system("cls");
			printf("Connect 4 closed, goodbye!\n");
			break;
		}
	} while (option != 6);

	return 0;
}