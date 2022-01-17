#include "AI.h"

bool undo(struct Hashmap** restrict board, struct Hashmap** restrict history);// , int* column);
bool redo(struct Hashmap** restrict board, struct Hashmap** restrict history);// , int* column);
void updateHistory(struct Hashmap** restrict history, const int column, const int token);
bool doOperation(struct Hashmap** restrict board, struct Hashmap** restrict history, const struct Settings* restrict settings, const int* restrict column, const int token, bool* restrict traversing, bool* restrict saving, const bool turn, const int AIOperator);
void displayBoard(const struct Hashmap* restrict board, const struct Matrix* restrict win);
void play(struct Hashmap** restrict loadedBoard, struct Hashmap** restrict loadedHistory, const struct Settings* restrict settings, const bool loadedTurn, const bool loadedTraversing);
