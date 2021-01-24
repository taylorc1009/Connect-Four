# Connect Four
Consists of both single player (player versus AI) and player versus player.

## AI features
The AI has been implemented using a scoring system which is recursively called while utilizing the **Minimax Algorithm**, an algorithm made to evaluate the outcome of every possible move that could be made within in the next *n* turns. The greater *n* is (which is known as the **depth**), the more moves the AI can predict.


### Minimax Algorithm
The Minimax Algorithm is essentially a decision tree, with each node being different possible states in the next *'depth'* moves.

It works by, firstly, making a copy of the board and making a move in the first column as the AI. Then, since the next turn is the players', it will make a move as the player (again in the first column), then the AI again. This will continue until *depth == 0*. Once it does, the algorithm will return the score deducted from this series of moves made in the copied board (i.e. at each node of the tree). The returned score *depth == 0* determines if each move was either good or bad for the AI or the player. Once it has the final score of this scenario, it plays the next move at *depth == 1* again but, this time, in the second column, then goes to *depth == 0* and evaluates the board again. Once it has the score here, the score returned from this node is compared to the evaluation before it and picks the best one (will be the lowest one for the player or the highest for the AI). It moves to the third column, fourth, fifth, and so on until the final column is reached. When it is, the algorithm then goes back to *depth == 2* (which is the players' turn) and moves their next move into the second column and starts making the same players' moves in every column again. This continues until the AI's first move (the move at *depth == {what the max depth was set as}* is in the last available column.

If, at any point during decision making, the algorithm reaches a Terminal Node, a score will be returned based on what the winning move is. A "Terminal Node" is a game ending move, which is either: the board is full, the AI won, or the player won. If the AI wins, a (positive) high score is returned, and the player; an overly negative number is returned. The score returned from these nodes, except when the board is full, varies depending on the depth it was found at.

The algorithm is very thorough but it quickly becomes a very large procedure as you increase the depth. The sum of it's recursions is given by calculating (board's length, x)^(depth). So, if we had the default connect 4 dimensions (7x6) with a depth of 7, the occurances of Minimax would become 7^7 (which equals 823,543). We can prevent this many occurances by implementing what is known as **Alpha-Beta Pruning**.


### Alpha-Beta Pruning
The Minimax Algorithm finds the best score of a generated series of moves, being the highest for one player (the AI) and the lowest for the other (the user). With this logic, we can cut the amount of occurances by determining if the current series of moves could be better than the previous ones or not. For example, say the AI found a series of moves where the score for the player is -30. Then, on the next, it finds early on during evaluation that the score can only be as low as -10. This means that we can eliminate (*prune*) this whole branch of evaluations as the algorithm already has a better move to overcome the player. With this, we can have a greater depth without having to sacrifice as much processing time.