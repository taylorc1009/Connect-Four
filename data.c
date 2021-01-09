#include "structs/Hashmap.h"
#include "game-structures.h"

bool saveGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool turn, bool traversing) {
	FILE* file;

	if (file = fopen("save.bin", "w")) {
		fprintf(file, "%c", settings->boardX + '0');
		fprintf(file, "%c", settings->boardY + '0');
		fprintf(file, ";");

		fprintf(file, "%c", ((int)turn) + '0');
		fprintf(file, ";");

		fprintf(file, "%c", ((int)traversing) + '0');
		fprintf(file, ";");

		for (int i = 0; i < strlen(settings->player1); i++)
			fprintf(file, "%c", settings->player1[i]);
		fprintf(file, ";");

		for (int i = 0; i < strlen(settings->player2); i++)
			fprintf(file, "%c", settings->player2[i]);
		fprintf(file, ";");

		fprintf(file, "%c", ((int)settings->solo) + '0');
		fprintf(file, ";");

		fprintf(file, "%c", settings->depth + '0');
		fprintf(file, ";");

		for (int i = 0; i < getX(*board); i++) {
			for (int j = 0; j < getY(*board); j++)
				fprintf(file, "%c", *((int*)getToken(*board, i, j)) + '0');
			fprintf(file, ";");
		}

		for (int i = 0; i < 2; i++) {
			fprintf(file, "%c", hashGet(*history, i)->size + '0');
			fprintf(file, ";");

			for (int j = 0; j < hashGet(*history, i)->size; j++) {
				struct Move* move = (struct Move*)getToken(*history, i, j);

				fprintf(file, "%c", move->column + '0');
				fprintf(file, "%c", move->token + '0');
				fprintf(file, ";");
			}
		}

		fclose(file);
	}
	else {
		printf("\n(!) file to save to was not found: 'save.bin'");
		return false;
	}

	return true;
}

bool loadGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool* turn, bool* traversing) {
	FILE* file;
	int step = 0, x = 0, y = 0, bufferSize = 300;
	bool isSecond = false; //we will use this to determine if the current value we're reading is the second of 2 digits, for example, X and Y
	char* buffer = malloc(sizeof(char) * bufferSize);
	struct Move* move;

	if (file = fopen("save.bin", "r")) {
		fseek(file, 0, SEEK_END);
		long len = (long)ftell(file);

		if (len > 0) {
			rewind(file);

			while (fgets(buffer, bufferSize, file) != NULL) {
				for (int i = 0; i < bufferSize; i++) {

					//this is a terrible way to read a file, but the save is just a bunch of different parameters for the gameplay and I did it this way to safely allow an increase
					//in data that ends up exceeding the buffer size
					//if the size does end up exceeding the buffer size, the parameters used (such as 'step' and 'isSecond') will allow it to continue from the exact same step of
					//the loading process with the next buffer
					if (buffer[i] != -1 && buffer[i] != 0) {
						//printf("%d: %d(%c) | step = %d, x = %d, y = %d\n", i, buffer[i], buffer[i], step, x, y);
						if (buffer[i] == ';') {
							if (step == 7 && x != settings->boardX - 1) {
								x++;
								y = 0;
							}
							else if (step == 9 && y == hashGet(*history, x)->size) {
								if (x == 1) {
									free(buffer);
									return true; //disables further reading of the input buffer after completion and signifies a successful read
								}
								else {
									step = 8;
									x++;
									y = 0;
								}
							}
							else if (step < 9) {
								step++;
								if (step < 8 || (step == 8 && x != 1))
									x = 0;
							}
						}
						else {
							if (step == 0) {
								if (i == 0) {
									settings->player1 = (char*)malloc(sizeof(char)); //this is not a memory leak, same goes for the other dynamically allocated variables in this list - they will always be NULL or have been previously free'd before this point
									memset(settings->player1, NUL, 1);

									settings->player2 = (char*)malloc(sizeof(char));
									memset(settings->player2, NUL, 1);

									*history = createTable(2, 0);
								}

								if (!isSecond)
									settings->boardX = (int)buffer[i] - '0';
								else {
									settings->boardY = (int)buffer[i] - '0';
									*board = createTable(settings->boardX, settings->boardY);
								}

								isSecond = !isSecond;
							}
							else if (step == 1)
								*turn = (bool)(buffer[i] - '0');
							else if (step == 2)
								*traversing = (bool)(buffer[i] - '0');
							else if (step == 3) {
								settings->player1 = (char*)realloc(settings->player1, sizeof(char) * x + 1);
								settings->player1[x] = buffer[i];
								settings->player1[x + 1] = '\0';
								x++;
							}
							else if (step == 4) {
								settings->player2 = (char*)realloc(settings->player2, sizeof(char) * x + 1);
								settings->player2[x] = buffer[i];
								settings->player2[x + 1] = '\0';
								x++;
							}
							else if (step == 5)
								settings->solo = (bool)(buffer[i] - '0');
							else if (step == 6)
								settings->depth = (int)(buffer[i] - '0');
							else if (step == 7) {
								if (y < settings->boardY && ((int)buffer[i] - '0') != 0) {
									int* tok = malloc(sizeof(int));
									*tok = (int)buffer[i] - '0';
									addMove(*board, x, tok);
								}
							}
							else if (step == 8) {
								resizeStack(hashGet(*history, x), (int)buffer[i] - '0');
							}
							else if (step == 9) {
								if (!isSecond) {
									move = (struct Move*)malloc(sizeof(struct Move));
									move->column = (int)buffer[i] - '0';
								}
								else {
									move->token = (int)buffer[i] - '0';

									push(hashGet(*history, x), &move);
									y++;
								}
								isSecond = !isSecond;
							}
						}
					}
				}
			}
		}
		else {
			printf("\n(!) there is no existing save\n");
			free(buffer);
			return false;
		}
	}
	else {
		printf("\n(!) file to load from was not found: 'save.bin'\n");
		free(buffer);
		return false;
	}

	printf("\n(!) failed to load the save correctly\n");
	free(buffer);
	return false;
}