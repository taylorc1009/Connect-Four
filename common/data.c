#include "data.h"

char* cancelLoad(struct Hashmap* board, struct Hashmap* history, struct Settings* settings, FILE* file) {
	if (board != NULL)
		freeHashmap(board);
	if (history != NULL)
		freeHashmap(history);

	if (settings != NULL) {
		if (settings->player1 != NULL)
			free(settings->player1);
		if (settings->player2 != NULL)
			free(settings->player2);
	}

	return FAILED_MESSAGE;
}

bool saveGame(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, bool turn, bool traversing) {
	FILE* file;

	if (file = fopen("save.bin", "wb")) { //should create the file if it doesn't exist, but this is here in case the file could not be created
		int x = settings->boardX, y = settings->boardY;
		fwrite(&x, sizeof(int), 1, file);
		fwrite(&y, sizeof(int), 1, file);
		
		for (int i = 0; i < x; i++) 
			for (int j = 0; j < y; j++)
				fwrite((int*)getToken(*board, i, j), sizeof(int), 1, file);

		fwrite(&(*history)->size, sizeof(int), 1, file);
		struct Stack* stack;

		for (int i = 0; i < (*history)->size; i++) {
			stack = hashGet(*history, i);
			fwrite(&stack->size, sizeof(int), 1, file);

			for (int j = 0; j < stack->size; j++) {
				/* I was previously trying to do this:
				 *     fwrite((struct Move*)stackGet(stack, j), sizeof(struct Move), 2, file);
				 * and read by doing:
				 *     fread(move, sizeof(struct Move), 2, file);
				 * which I've read does work but, when I tried, the reading stage would crash: what was wrong? */
				struct Move* move = (struct Move*)stackGet(stack, j);
				fwrite(&move->column, sizeof(int), 1, file);
				fwrite(&move->token, sizeof(int), 1, file);
			}
		}

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

	if (file = fopen("save.bin", "rb")) {
		fseek(file, 0, SEEK_END);
		long len = (long)ftell(file);

		if (len > 0) {
			rewind(file);

			if (!fread(&settings->boardX, sizeof(int), 1, file) || !fread(&settings->boardY, sizeof(int), 1, file))
				return cancelLoad(NULL, NULL, NULL, file);
			int x = settings->boardX, y = settings->boardY;

			*board = createTable(x, y);
			
			for (int i = 0; i < x; i++) {
				for (int j = 0; j < y; j++) {
					int buffer;
					if (!fread(&buffer, sizeof(int), 1, file))
						return cancelLoad(*board, NULL, NULL, file);

					if (buffer != EMPTY_SLOT) {
						int* token = malloc(sizeof(int));
						*token = buffer;
						if (!addMove(*board, i, token)) {
							free(token);
							return cancelLoad(*board, *history, NULL, file);
						}
					}
				}
			}
			
			int nlogs;
			if (!fread(&nlogs, sizeof(int), 1, file))
				return cancelLoad(*board, NULL, NULL, file);

			*history = createTable(nlogs, 0);

			struct Stack* stack;
			int size;

			for (int i = 0; i < nlogs; i++) {
				if (!fread(&size, sizeof(int), 1, file))
					return cancelLoad(*board, *history, NULL, file);
				
				stack = hashGet(*history, i);
				resizeStack(stack, size);

				for (int j = 0; j < size; j++) {
					struct Move* move = (struct Move*)malloc(sizeof(struct Move));
					if (!fread(&move->column, sizeof(int), 1, file) || !fread(&move->token, sizeof(int), 1, file))
						return cancelLoad(*board, *history, NULL, file);

					if (!push(stack, (void**)&move)) { //push failsafe, it shouldn't fail as the correct size should be used but just in case
						free(move);
						return cancelLoad(*board, *history, NULL, file);
					}
				}
			}

			if (!fread(&settings->depth, sizeof(int), 1, file) || !fread(&settings->solo, sizeof(bool), 1, file))
				return cancelLoad(*board, *history, NULL, file);

			if (!fread(&size, sizeof(int), 1, file))
				return cancelLoad(*board, *history, NULL, file);
			settings->player1Size = size;

			settings->player1 = (char*)malloc(sizeof(char) * size + 1);
			if (!fread(settings->player1, sizeof(char), settings->player1Size, file))
				return cancelLoad(*board, *history, settings, file);
			settings->player1[settings->player1Size] = '\0'; //fixes the lack of a string terminator

			if (!fread(&size, sizeof(int), 1, file))
				return cancelLoad(*board, *history, settings, file);
			settings->player2Size = size;

			settings->player2 = (char*)malloc(sizeof(char) * size + 1);
			if (!fread(settings->player2, sizeof(char), settings->player2Size, file))
				return cancelLoad(*board, *history, settings, file);
			settings->player2[settings->player2Size] = '\0';

			if (!fread(turn, sizeof(bool), 1, file) || !fread(traversing, sizeof(bool), 1, file))
				return cancelLoad(*board, *history, settings, file);

			fclose(file);

			return NULL;
		}
		else {
			fclose(file);
			return NONE_EXISTING_MESSAGE;
		}
	}
	else
		return NOT_FOUND_MESSAGE;
}