/* TODO
*	Implement:
*	- Finish internal commentary
*	- Randomize (or allow selection) of player or AI turn on first move?
*	- Test save and load more
*	- Test undo and redo more
*	- Look at if infinitely long player names are possible (using stdin and realloc)
*	- Add a move down animation?
*	- Implement CMake's security suggestions
*
*	-- GCC TODO - I have found that the 'free(win[i])' at the bottom of 'play()' in 'game.c' is crashing, I'm assuming that the 'displayBoard()' function is invalidating it, but I'm insure if it is also deallocating it because I can still acces the matrixs' values from 'play()'
*				- I have also created a 'CLEAR_TERMINAL' constant with the command to to clear the terminal based on the OS, but I am not using it yet as I'm still trying to debug the 'free()' problem from above
*				- 'delay()' needs to be fixed: I am unsure if the function 'clock()' is working the same as in Windows
*				- UPDATE: check the bottom of the 'play' method in 'game.c'
*
*	Update README.md upon completion (if required)
*/

#include "menu.h"

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
	char* buffer = (char*)malloc(sizeof(char) * NAME_BUFFER_MAX);
	int size;

	//comments here were an attempt to get an infinitely long string: one that exceeds the buffer size
	//char* input = (char*)malloc(sizeof(char));
	//int i = 0;
	//bool proceed = false;
	
	memset(buffer, NUL, NAME_BUFFER_MAX);
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
		input = fgets(buffer, NAME_BUFFER_MAX, stdin);

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
	printf("\n%sPlayer 1%s, please enter your name\n> ", PLAYER_1_COLOUR, DEFAULT_COLOUR);
	settings->player1 = (char*)malloc(sizeof(char) * NAME_BUFFER_MAX);
	settings->player1Size = getName(&(settings)->player1);

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
		printf("\n%sPlayer 2%s, please enter your name\n> ", PLAYER_2_COLOUR, DEFAULT_COLOUR);
		settings->player2 = (char*)malloc(sizeof(char) * NAME_BUFFER_MAX);
		settings->player2Size = getName(&(settings)->player2);
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
	//remove this and you'll notice that 'printf' outputs are buffered (they happen after delays, terminal clears...), but it only happend on Unix by default
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