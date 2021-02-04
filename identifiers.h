#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//we use a typedef instead of '#include<stdbool.h>' as "bool" is not a key-word to the compiler
//so including it when we use it as a return type gives errors
typedef enum { false, true } bool;

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2
#define EMPTY_SLOT 0

//#define ARRAY_LENGTH(x) (sizeof(x) / sizeof((x)[0])) //we don't use this because we cannot calculate the size of a dynamic array; the compiler can never know its size and this will only give the size of the pointer

#define NUL '\0' //used to "nullify" a char

//C console colours (source - https://stackoverflow.com/a/3586005/11136104)
#define PLAYER_1_COLOUR "\x1B[31m" //red
#define PLAYER_2_COLOUR "\x1B[33m" //yellow
#define WIN_COLOUR "\x1B[32m" //green
#define DEFAULT_COLOUR "\x1B[0m" //default console text color

#if _WIN32
#define CLEAR_TERMINAL "cls"
#else //elif __linux__ || __unix__
#define CLEAR_TERMINAL "clear"
#endif