#include "AI.h"

bool undo(struct Hashmap** board, struct Hashmap** history, int* column);
bool redo(struct Hashmap** board, struct Hashmap** history, int* column);
void updateHistory(struct Hashmap** history, int column, int p);
bool doOperation(struct Hashmap** board, struct Hashmap** history, struct Settings* settings, int* column, int token, bool* traversing, bool* saving, bool turn, int AIOperator);
void displayBoard(struct Hashmap* board, struct Matrix* win);
void play(struct Hashmap** loadedBoard, struct Hashmap** loadedHistory, struct Settings* settings, bool loadedTurn, bool loadedTraversing);
