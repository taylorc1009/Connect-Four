#include "structs/Hashmap.h"
#include "game-structures.h"

#define FAILED_MESSAGE "\n(!) failed to load the save correctly\n"
#define NOT_FOUND_MESSAGE "\n(!) file to load from was not found: 'save.bin'\n"
#define NONE_EXISTING_MESSAGE "\n(!) there is no existing save\n"

char* cancelLoad(struct Hashmap* board, struct Hashmap* history, FILE* file) {
	if (board != NULL)
		freeHashmap(board);
	if (history != NULL)
		freeHashmap(history);

	return FAILED_MESSAGE;
}

bool saveGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool turn, bool traversing) {
	FILE* file;

	if (file = fopen("save.bin", "wb")) {
		/*fprintf(file, "%c", settings->boardX + '0');
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
		
		fclose(file);*/

		int x = settings->boardX, y = settings->boardY;
		fwrite(&x, sizeof(int), 1, file);
		fwrite(&y, sizeof(int), 1, file);
		
		for (int i = 0; i < x; i++) {
			//fwrite(&(*board)->list[i]->key, sizeof(int), 1, file);

			struct Stack* stack = hashGet(*board, i);
			//fwrite(&stack->top, sizeof(int), 1, file);
			for (int j = 0; j < y; j++)
				fwrite((int*)getToken(*board, i, j), sizeof(int), 1, file);//stack->list[j]->val
		}

		fwrite(&(*history)->size, sizeof(int), 1, file);
		struct Stack* stack;

		for (int i = 0; i < (*history)->size; i++) {
			stack = hashGet(*history, i);
			fwrite(&stack->size, sizeof(int), 1, file);

			//fwrite(&(*history)->list[0]->key, sizeof(int), 1, file);
			//fwrite(&stack->top, sizeof(int), 1, file);
			for (int j = 0; j < stack->size; j++)
				fwrite((struct Move*)stackGet(stack, j), sizeof(struct Move), 2, file);//stack->list[j]->val
		}

		/*stack = hashGet(*history, 1);
		fwrite(&stack->size, sizeof(int), 1, file);

		//fwrite(&(*history)->list[1]->key, sizeof(int), 1, file);
		//fwrite(&stack->top, sizeof(int), 1, file);
		for (int i = 0; i < stack->size; i++)
			fwrite((struct Move*)getToken(*board, 1, i), sizeof(struct Move), 1, file);//stack->list[j]->val*/

		//we need to read and write each setting individually as the player name is a pointer to an array
		fwrite(&settings->depth, sizeof(int), 1, file);
		fwrite(&settings->solo, sizeof(bool), 1, file);

		fwrite(&settings->player1Size, sizeof(int), 1, file);
		fwrite(settings->player1, sizeof(char), settings->player1Size, file);

		fwrite(&settings->player2Size, sizeof(int), 1, file);
		fwrite(settings->player2, sizeof(char), settings->player2Size, file);

		fwrite(&turn, sizeof(bool), 1, file);
		fwrite(&traversing, sizeof(bool), 1, file);

		fclose(file);
	}
	else {
		printf("\n(!) file to save to was not found: 'save.bin'");
		return false;
	}

	return true;
}

char* loadGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool* turn, bool* traversing) {
	FILE* file;
	char* error;
	//int step = 0, x = 0, y = 0, bufferSize = 300;
	//bool isSecond = false; //we will use this to determine if the current value we're reading is the second of 2 digits, for example, X and Y
	//char* buffer = malloc(sizeof(char) * bufferSize);
	//struct Move* move;

	if (file = fopen("save.bin", "rb")) {
		fseek(file, 0, SEEK_END);
		long len = (long)ftell(file);

		if (len > 0) {
			rewind(file);

			if (!fread(&settings->boardX, sizeof(int), 1, file) || !fread(&settings->boardY, sizeof(int), 1, file))
				return cancelLoad(NULL, NULL, file);
			int x = settings->boardX, y = settings->boardY;

			*board = createTable(x, y);

			for (int i = 0; i < x; i++) {
				//fread(&(*board)->list[i]->key, sizeof(int), 1, file);

				//struct Stack* s = hashGet(*board, i);
				//fread(&s->top, sizeof(int), 1, file);
				for (int j = 0; j < y; j++) {
					int buffer;

					if (!fread(&buffer, sizeof(int), 1, file))
						return cancelLoad(*board, NULL, file);

					if (buffer != EMPTY_SLOT) {
						int* token = malloc(sizeof(int));
						*token = buffer;
						addMove(*board, i, token);
					}
					//else
						//break; //there won't be any player tokens after the first empty slot in this column, so end the add for this column
				}
			}

			int nlogs;
			if (!fread(&nlogs, sizeof(int), 1, file))
				return cancelLoad(*board, NULL, file);
			*history = createTable(nlogs, 0);

			struct Stack* stack;
			int size;

			for (int i = 0; i < nlogs; i++) {
				stack = hashGet(*history, i);
				if (!fread(&size, sizeof(int), 1, file))
					return cancelLoad(*board, *history, file);
				resizeStack(stack, size);

				//fread(&(*history)->list[0]->key, sizeof(int), 1, file);
				//fread(&stack->top, sizeof(int), 1, file);
				for (int i = 0; i < size; i++) {
					struct Move* move = (struct Move*)malloc(sizeof(struct Move));
					if (!fread(move, sizeof(struct Move), 2, file))
						return cancelLoad(*board, *history, file);
					push(stack, &move);
				}
			}

			/*stack = hashGet(*history, 1);
			fread(size, sizeof(int), 1, file);
			resizeStack(stack, size);

			//fwrite(&(*history)->list[1]->key, sizeof(int), 1, file);
			//fwrite(&stack->top, sizeof(int), 1, file);
			for (int i = 0; i < size; i++) {
				struct Move* move = (struct Move*)malloc(sizeof(struct Move*));
				fread(move, sizeof(struct Move), 1, file);//stack->list[j]->val
				push(stack, &move);
			}*/

			if (!fread(&settings->depth, sizeof(int), 1, file) || !fread(&settings->solo, sizeof(bool), 1, file))
				return cancelLoad(*board, *history, file);

			if (!fread(&size, sizeof(int), 1, file))
				return cancelLoad(*board, *history, file);
			settings->player1Size = size;
			settings->player1 = (char*)malloc(sizeof(char) * size);//this is a memory leak if the search fails: we still need to free it in that instance
			if (!fread(settings->player1, sizeof(char), settings->player1Size, file))
				return cancelLoad(*board, *history, file);

			if (!fread(&size, sizeof(int), 1, file))
				return cancelLoad(*board, *history, file);
			settings->player2Size = size;
			settings->player2 = (char*)malloc(sizeof(char) * size);
			if (!fread(settings->player2, sizeof(char), settings->player2Size, file))
				return cancelLoad(*board, *history, file);

			if (!fread(turn, sizeof(bool), 1, file) || !fread(traversing, sizeof(bool), 1, file))
				return cancelLoad(*board, *history, file);

			fclose(file);

			return NULL;

			/*while (fgets(buffer, bufferSize, file) != NULL) {
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

			fclose(file);*/
		}
		else {
			fclose(file);
			return NONE_EXISTING_MESSAGE;
			//free(buffer);
			//return false;
		}
	}
	else
	return NOT_FOUND_MESSAGE;
		//free(buffer);
		//return false;

	//free(buffer);
	return FAILED_MESSAGE;
}