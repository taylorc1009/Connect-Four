#include <stdio.h>
#include <stdlib.h>

//we use a typedef instead of '#include<stdbool.h>' as "bool" is not a key-word to the compiler
//so including it when we use it as a return type gives errors
typedef enum { false, true } bool;

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

#define ARRAY_LENGTH(x) ((int)sizeof(x) / (int)sizeof((x)[0])) //we use this because we cannot calculate the size of a dynamic array; the compiler can never know its size

#define NUL '\0' //used to "nullify" a char