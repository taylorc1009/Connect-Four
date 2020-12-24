//we use a typedef instead of '#include<stdbool.h>' as "bool" is not a key-word to the compiler
//so including it when we use it as a return type gives errors
typedef enum { false, true } bool;

#define PLAYER_1_TOKEN 1
#define PLAYER_2_TOKEN 2