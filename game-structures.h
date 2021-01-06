struct Settings {
	int boardX;
	int boardY;
	char* player1;
	char* player2;
	bool solo;
	int depth;
};

struct Move {
	int column;
	int token;
};