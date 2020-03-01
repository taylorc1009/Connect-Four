#include <stdio.h>

typedef enum { false, true } bool;

int main(int argc, char **argv)
{
	system("cls");
	int boardX = 9;
	int boardY = 7;
	printf("\nWelcome to Connect 4! Reproduced virtually using C by Taylor Courtney - 40398643\nTo begin, select either:\n\n 1 - how it works\n 2 - change board size (currently %dx%d)\n 3 - start Player versus Player\n 4 - start Player versus AI\n 5 - quit\n\nWhere would you like to start?\n> ", boardX, boardY);
	int option = validateOption();
	system("cls");
	return 0;
}

int validateOption() {
	bool valid = false;
	int num = 0;
	while (!valid) { //invalid response causes an infonite loop
		char term;
		if (scanf("%d%c", &num, &term) != 2 || term != '\n' || !(num >= 1 && num <= 5))
			printf("\n! invalid input: please re-enter\n> ");
		else
			valid = true;
	}
	return num;
}