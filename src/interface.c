/**
 * @file
 * @brief Implementation of the server
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>

#include "board.h"
#include "interface.h"
#include "display.h"

// to avoid warnings
#define MARK_USED(name__) while(0) \
	name__ = name__

// these static functions will only
// be visible in this file except
// for the tests
#ifndef TEST
#define STATIC static
#else
#define STATIC 
#endif

STATIC struct board bo;
extern FILE *gameRecord;

STATIC int block_player(const struct board *b,
			char column, char line,
			char direction);

STATIC int is_goal(char line, int player);


STATIC int block_player__(const struct board *b,
			  char column, char line,
			  char direction, int player);

STATIC int block_player__R(const struct board *b,
			   char column, char line,
			   int player, int mem[]);

STATIC int wall_intersect(char column, char line,
			  char direction);

STATIC int wall_overlap(char column, char line,
			char direction);

STATIC int has_wall(char column, char line,
		    char direction);

STATIC int is_valid_pawn_target(const struct board *b,
				char col, char line, char dir);

/*----------- INTERFACE ----------------*/
/**
 * @brief Find out who is the current player
 * @param[in] b the useless board
 * @return an integer which is either 1(WHITE) or 1 (BLACK) 
 */
unsigned int get_current_player(const struct board *b)
{
    MARK_USED(b);
    return bo.currentPlayer;
}

/**
 * Returns an integer describing what is
 * at position (column,line) on
 * the board b.
 * The result is either -1 (empty),
 * 0 (white) or 1 (black).
 */
int get_position(const struct board* b,
		 char column, char line)
{
    MARK_USED(b);
    if (bo.players[BLACK].col == column &&
	bo.players[BLACK].line == line)
	return BLACK;
    if (bo.players[WHITE].col == column &&
	bo.players[WHITE].line == line)
	return WHITE;
    return EMPTY;
}

/**
 * Returns an integer indicating whether there is a wall
 * on the board b at position starting from (column,line),
 * in a direction that is either towards
 * smaller cols (0), smaller lines (1),
 * larger cols (2) or larger lines (3).
 */
int is_passable(const struct board* b, 
		char column, char line,
		char direction)
{
    MARK_USED(b);

    switch (direction) {
    case UP:
	return !has_wall(column, line-1, HORIZONTAL) &&
	    !has_wall(column-1, line-1, HORIZONTAL);
	break;
    case DOWN:
	return !has_wall(column, line, HORIZONTAL) &&
	    !has_wall(column-1, line, HORIZONTAL);
	break;
    case RIGHT:
	return !has_wall(column, line, VERTICAL) &&
	    !has_wall(column, line-1, VERTICAL);
	break;
    case LEFT:
	return !has_wall(column-1, line, VERTICAL) &&
	    !has_wall(column-1, line-1, VERTICAL);
	break;
    }
    return 0;
}

/**
 * Moves a pawn on the position (column,line)
 * Columns and lines are numbered from 0 to N.
 */
void move_pawn(const struct board* b,
	       char column, char line)
{

    MARK_USED(b);
    
    int currentCol = bo.players[bo.currentPlayer].col;
    int currentLine = bo.players[bo.currentPlayer].line;
    int dcol = column - currentCol;
    int dline = line - currentLine;
    int dir = -1;
    
    switch (dcol) {
	case -1:
	    switch(dline) {
		case -1:
		    dir = UPLEFT;
		    break;
		case 1:
		    dir = DOWNLEFT;
		    break;
		case 0:
		    dir = LEFT;
		    break;
	    }
	    break;
	case 1:
	    switch(dline) {
		case -1:
		    dir = UPRIGHT;
		    break;
		case 1:
		    dir = DOWNRIGHT;
		    break;
		case 0:
		    dir = RIGHT;
		    break;
	    }
	    break;
	case 0:
	    switch(dline) {
		case -1:
		    dir = UP;
		    break;
		case 1:
		    dir = DOWN;
		    break;
		case -2:
		    dir = UPUP;
		    break;
		case 2:
		    dir = DOWNDOWN;
		    break;
	    }
	    break;
	case 2:
	    switch(dline) {
		case 0:
		    dir = RIGHTRIGHT;
		    break;
	    }
	    break;
	case -2:
	    switch(dline) {
		case 0:
		    dir = LEFTLEFT;
		    break;
	    }
	    break;
    }

    int authorizedMove = is_valid_pawn_target(b, currentCol, currentLine
					      ,dir);
    
    // the last player and the current player
    // must not be the same (otherwise it means
    // a player play twice)
    authorizedMove = authorizedMove && 
	bo.lastPlayer != bo.currentPlayer;

    if (authorizedMove) {
	put_in_case(bo.players[bo.currentPlayer].col,
		    bo.players[bo.currentPlayer].line,
		    " ");
	put_in_case(column, line,
		    bo.currentPlayer == WHITE ? "W" : "B");
    
	bo.players[bo.currentPlayer].col = column;
	bo.players[bo.currentPlayer].line = line;
#ifdef PRINT_IN_FILE
	fprintf(gameRecord, "%c%d ",
		'a' + column, line+1);
#endif
	bo.lastPlayer = bo.currentPlayer;
    } else {
#ifndef TEST
	fprintf(stderr, "\r\n\rMouvement interdit\n\r"
		"%s %d %d\n\r",
		bo.currentPlayer == WHITE?"WHITE":"BLACK",
		column, line);
#endif
#ifdef USE_NCURSES
	getch();
	endwin();
#endif
#ifndef TEST
	exit(EXIT_FAILURE);
#endif
    }
}

/**
 * Returns the number of available WALLS!!!!bridges for the current player.
 */
unsigned int remaining_bridges(const struct board* b,
			       unsigned int player)
{
    MARK_USED(b);
    return MAX_WALLS_PER_PLAYER - bo.nbWallsPlayer[player];
}

/**
 * Returns an integer indicating whether it is possible
 * to place a wall on the corresponding
 * column, line and direction.
 * Directions are either 0 (horizontal) or 1 (vertical).
 */
int is_blockable(const struct board* b,
			char column, char line,
			char direction)
{
    return (!has_wall(column, line, direction) &&
	    !wall_intersect(column, line, direction) &&
	    !wall_overlap(column, line, direction) &&
	    !block_player(b, column, line, direction));
}

/**
 * Places a wall on the corresponding column, line and direction
 * Directions are either 0 (horizontal) or 1 (vertical).
 */
void place_wall(const struct board *b,
		char column, char line,
		char direction)
{
    if (is_blockable(b, column, line, direction) &&
	bo.nbWallsPlayer[bo.currentPlayer]
	< MAX_WALLS_PER_PLAYER
    && bo.lastPlayer != bo.currentPlayer) {
	bo.walls[bo.nbWalls].col = column;
	bo.walls[bo.nbWalls].line = line;
	bo.walls[bo.nbWalls].dir = direction;
	
	++ bo.nbWallsPlayer[bo.currentPlayer];
	++ bo.nbWalls;
       
	print_wall(column, line, direction);
	print_remaining_walls();
	
	#ifdef PRINT_IN_FILE
	fprintf(gameRecord, "%c%d%c ",
		'a' + column,
		line+1,
		direction == VERTICAL ? 'v' : 'h');
	#endif
	bo.lastPlayer = bo.currentPlayer;
    } else {
#ifndef TEST
	fprintf(stderr, "\r%s player: error_place wall\n\r%d %d %s",
		bo.currentPlayer == BLACK?"BLACK":"WHITE",
		column,
		line,
		direction == VERTICAL? "VERTICAL":"HORIZONTAL");
#endif
#ifdef USE_NCURSES
	getch();
	endwin();
#endif
#ifndef TEST
	exit(EXIT_FAILURE);
#endif
    }
}
/* ---------- SERVER ---------------*/

/**
 * @brief Initialize the game 
 */
void
init_board(void)
{
    bo.nbWalls = 0;
    bo.nbWallsPlayer[BLACK] = 0;
    bo.nbWallsPlayer[WHITE] = 0;
	
    bo.players[BLACK].col = E;
    bo.players[BLACK].line = 8;

    bo.players[WHITE].line = 0;
    bo.players[WHITE].col = E;
    bo.currentPlayer = BLACK; // a random starting player
    bo.lastPlayer = 1 - bo.currentPlayer;
}

/**
 * @brief Tells if game is finished or not.
 * @return boolean telling if game is finished or not.
 */
int is_finished(void)
{
    return (is_goal(bo.players[WHITE].line, WHITE) ||
	    is_goal(bo.players[BLACK].line, BLACK));
}

/**
 * @brief Compute the player who is supposed to play next.
 */
void
switch_player(void)
{
    bo.currentPlayer = 1 - bo.currentPlayer;
}

/*-------INTERFACE IMPLEMENTS PRIVATE-------------*/

/**
 * @brief Tells if the wall is blocking a player
 * @param[in] b a pointer to whatever location you want ...
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @param[in] direction the direction of the wall
 * 1 (VERTICAL), or 0 (HORIZONTAL).
 * @return boolean saying if a player is blocked
 *
 * Return if positionning a wall
 * at (column, line, direction)
 * would block any player or nobody
 */
STATIC int block_player(const struct board *b,
		 char column, char line,
		 char direction)
{
    return (block_player__(b, column, line, direction, BLACK) ||
	    block_player__(b, column, line, direction, WHITE));
}

/**
 * @brief Tells if a line is the player's goal
 * @param[in] line the line which interest us
 * @param[in] player the player which interest us
 * @return boolean saying if line is player's goal
 *
 * return whether player who is on positioned on a certain
 * line reached its goal or not
 */
STATIC int is_goal(char line, int player)
{
    return ((player == BLACK &&
	     line == 0) ||
	    (player == WHITE &&
	     line == BOARD_SIZE-1));
}


#define VALUE(matrix__, x__, y__)    ((matrix__)[(x__) * BOARD_SIZE + (y__)])

/**
 * @brief Tells if the wall is blocking the player
 * @param[in] b a pointer to whatever location you want ...
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @param[in] direction the direction of the wall
 * 1 (VERTICAL), or 0 (HORIZONTAL).
 * @param[in] player the player which interest us
 * @return boolean saying if the player is blocked
 *
 * return whether positionning a wall
 * (column, line, direction)
 * would block the given player or not
 */
STATIC int block_player__(const struct board *b,
		   char column, char line,
		   char direction, int player)
{
    int mem[BOARD_SIZE * BOARD_SIZE] = { 0 };

    bo.walls[bo.nbWalls].col = column;
    bo.walls[bo.nbWalls].line = line;
    bo.walls[bo.nbWalls].dir = direction;
    ++ bo.nbWalls;

    char playerCol = bo.players[player].col;
    char playerLine = bo.players[player].line;
    int ret = block_player__R(b, playerCol, playerLine,
			      player, mem);
    -- bo.nbWalls;
    return ret;
}

/**
 * @brief Recursive auxiliarry function for block_player__
 * @param[in] b a pointer to whatever location you want ...
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @param[in] player the player which interest us
 * @param[in,out] mem a memory we need to not retest
 * where we already went
 * @return boolean saying if the player is blocked
 *
 * Recursively enter in all squares near to
 * square (column, line)
 * and mark the ones it already enters
 * return 0 as soon as goal is reached,
 * and 1 if no square is reachable,
 * or if the square was already entered
 */
STATIC int block_player__R(const struct board *b,
			   char column, char line,
			   int player, int mem[])
{
    int ret = 1;
    if (VALUE(mem, column, line) == 1)
	return 1;
    if (is_goal(line, player))
	return 0;
    VALUE(mem, column, line) = 1;

    if (is_passable(b, column, line, RIGHT)) {
	ret = block_player__R(b, column+1, line,
			      player, mem);
	if (!ret)
	    return ret;
    }
    if (is_passable(b, column, line, UP)) {
	ret = block_player__R(b, column, line-1,
			      player, mem);
	if (!ret)
	    return ret;
    }
    if (is_passable(b, column, line, LEFT)) {
	ret = block_player__R(b, column-1, line,
			      player, mem);
	if (!ret)
	    return ret;
    }
    if (is_passable(b, column, line, DOWN))
	ret = block_player__R(b, column, line+1,
			      player, mem);
    return ret;
}

/**
 * @brief Tells if wall intersect with an existing wall.
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @param[in] direction the direction of the wall
 * @return boolean saying if the wall intersect
 * with an existing wall
 *
 * Return whether positionning a wall the place
 * (column, line) would make the wall intersect
 * with an existing wall or not
 */
STATIC int wall_intersect(char column, char line,
			  char direction)
{
    return (has_wall(column, line, 1-direction));
}

/**
 * @brief Tells if wall overlap with an existing wall.
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @param[in] direction the direction of the wall
 * @return boolean saying if the wall overlaps
 *
 * Return whether positionning a wall the place
 * (column, line) would make the wall overlap
 * with an existing wall or not
 */
STATIC int wall_overlap(char column, char line,
			char direction)
{
    return ((direction == VERTICAL &&
	     (has_wall(column, line-1, VERTICAL) ||
	      has_wall(column, line+1, VERTICAL))) ||
	    (direction == HORIZONTAL &&
	     (has_wall(column-1, line, HORIZONTAL)||
	      has_wall(column+1, line, HORIZONTAL))));
}

/**
 * @brief Tells if there is a wall
 * @param[in] column the column of the wall [0-8]
 * @param[in] line the line of the wall [0-8]
 * @return boolean saying if there is a wall
 *
 * Return whether there already is a wall
 * here or not
 */
STATIC int has_wall(char column, char line,
	     char direction)
{
    int i;
	
    //borders of board are considered as walls:
    if (((column < 0 || column >= BOARD_SIZE-1) &&
	 direction == VERTICAL) ||
	((line < 0 || line >= BOARD_SIZE-1) &&
	 direction == HORIZONTAL))
	return 1;

    for (i = 0; i < bo.nbWalls; ++i) {
	if (bo.walls[i].col == column &&
	    bo.walls[i].line == line &&
	    bo.walls[i].dir == direction)
	    return 1;
    }
    return 0;
}

STATIC int
is_valid_pawn_target(const struct board *b,
		     char col, char line, char dir)
{
    if (dir == -1)
	return 0;
    
    switch (dir) {
    case UP:
	return (line > 0 &&
		is_passable(b,
			    col, line, UP)  &&
		EMPTY == get_position(b, col, line-1));
	break;
    case DOWN:
	return (line < BOARD_SIZE-1 &&
		is_passable(b,
			    col, line, DOWN)  &&
		EMPTY == get_position(b, col, line+1));
	break;
    case LEFT:
	return (col > 0 &&
		is_passable(b,
			    col, line, LEFT)  &&
		EMPTY == get_position(b, col-1, line));
	break;
    case RIGHT:
	return (col < BOARD_SIZE-1 &&
		is_passable(b,
			    col, line, RIGHT)  &&
		EMPTY == get_position(b, col+1, line));
	break;
	
    case LEFTLEFT:
	return (col > 1 &&
		EMPTY != get_position(b, col-1, line) &&
		is_passable(b,
			    col, line,
			    LEFT)  &&
		is_passable(b,
			    col-1, line,
			    LEFT) );
	break;
    case UPUP:
	return (line > 1 &&
		EMPTY != get_position(b, col, line-1) &&
		is_passable(b,
			    col, line,
			    UP)  &&
		is_passable(b,
			    col, line-1,
			    UP) );
	break;
    case RIGHTRIGHT:
	return (col < BOARD_SIZE-2 &&
		EMPTY != get_position(b, col+1, line) &&
		is_passable(b,
			    col, line,
			    RIGHT)  &&
		is_passable(b,
			    col+1, line,
			    RIGHT) );
	break;
    case DOWNDOWN:
	return (line < BOARD_SIZE-2 &&
		EMPTY != get_position(b, col, line+1) &&
		is_passable(b,
			    col, line,
			    DOWN)  &&
		is_passable(b,
			    col, line+1,
			    DOWN) );
	break;
    case UPLEFT:
	return (line > 0 &&
		col > 0 &&
		((EMPTY != get_position(b, col, line-1) &&
		  is_passable(b,
			      col, line,
			      UP)  &&
		  is_passable(b,
			      col, line-1,
			      UP)  == 0 &&
		  is_passable(b,
			      col, line-1,
			      LEFT) ) ||
		 (EMPTY != get_position(b, col-1, line) &&
		  is_passable(b,
			      col, line,
			      LEFT)  &&
		  is_passable(b,
			      col-1, line,
			      LEFT)  == 0 &&
		  is_passable(b,
			      col-1, line,
			      UP)  )));
	break;
    case UPRIGHT:
	return (line > 0 &&
		col < BOARD_SIZE-1 &&
		((EMPTY != get_position(b, col, line-1) &&
		  is_passable(b,
			      col, line,
			      UP)  &&
		  is_passable(b,
			      col, line-1,
			      UP)  == 0 &&
		  is_passable(b,
			      col, line-1,
			      RIGHT) ) ||
		 (EMPTY != get_position(b, col+1, line) &&
		  is_passable(b,
			      col, line,
			      RIGHT)  &&
		  is_passable(b,
			      col+1, line,
			      RIGHT)  == 0 &&
		  is_passable(b,
			      col+1, line,
			      UP)  )));
	break;
    case DOWNLEFT:
	return (line < BOARD_SIZE-1 &&
		col > 0 &&
		((EMPTY != get_position(b, col-1, line) &&
		  is_passable(b,
			      col, line,
			      LEFT)  &&
		  is_passable(b,
			      col-1, line,
			      LEFT)  == 0 &&
		  is_passable(b,
			      col-1, line,
			      DOWN) ) ||
		 (EMPTY != get_position(b, col, line+1) &&
		  is_passable(b,
			      col, line,
			      DOWN)  &&
		  is_passable(b,
			      col, line+1,
			      DOWN)  == 0 &&
		  is_passable(b,
			      col, line+1,
			      LEFT)  )));
	break;
    case DOWNRIGHT:
	return (line < BOARD_SIZE-1 &&
		col < BOARD_SIZE-1 &&
		((EMPTY != get_position(b, col+1, line) &&
		  is_passable(b,
			      col, line,
			      RIGHT)  &&
		  is_passable(b,
			      col+1, line,
			      RIGHT)  == 0 &&
		  is_passable(b,
			      col+1, line,
			      DOWN) ) ||
		 (EMPTY != get_position(b, col, line+1) &&
		  is_passable(b,
			      col, line,
			      DOWN)  &&
		  is_passable(b,
			      col, line+1,
			      DOWN)  == 0 &&
		  is_passable(b,
			      col, line+1,
			      RIGHT)  )));
	break;
    }
    return 0;
}
