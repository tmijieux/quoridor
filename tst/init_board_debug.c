#include <board.h>

extern struct board bo;

void
init_board_debug(int nbWalls,
		 char currentPlayer, char lastPlayer,
		 char cpCol, char cpLine, 
		 char lpCol, char lpLine)
{
    bo.nbWalls = 0;
    bo.nbWallsPlayer[(int)currentPlayer] = 0;
    bo.nbWallsPlayer[(int)lastPlayer] = 0;
	
    bo.players[(int)currentPlayer].col = cpCol;
    bo.players[(int)currentPlayer].line = cpLine;

    bo.players[(int)lastPlayer].col = lpCol;
    bo.players[(int)lastPlayer].line = lpLine;

    bo.currentPlayer = currentPlayer;
    bo.lastPlayer = lastPlayer;
}

void place_wall_debug(char col, char line, char dir)
{
    bo.walls[bo.nbWalls].col = col;
    bo.walls[bo.nbWalls].line = line;
    bo.walls[bo.nbWalls].dir = dir;
    ++ bo.nbWalls;
}
