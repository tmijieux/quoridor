/**
 * @file
 * @brief Interface players with the server
 */

#ifndef INTERFACE_H
#define INTERFACE_H

struct board;

// Returns the ID of current player : 0 (white) or 1 (black).
unsigned int get_current_player(const struct board *b);

// Returns an integer describing what is at position (column,line)
// on the board b. The result is either -1 (empty), 0 (white) or 1 (black).
int get_position(const struct board* b,
		 char column, char line);

// Returns an integer indicating whether there is a wall on the board b
// at position starting from (column,line), in a direction that is either 
// towards smaller cols (0), smaller lines (1), larger cols (2) or larger lines (3).
int is_passable(const struct board* b,
		char column, char line,
		char direction);

// Moves a pawn on the position (column,line)
// Columns and lines are numbered from 0 to N.
void move_pawn(const struct board* b,
	       char column, char line); 

// Returns the number of available bridges for the current player. 
unsigned int remaining_bridges(const struct board* b, unsigned int player);

// Returns an integer indicating whether it is possible to place a wall
// on the corresponding column, line and direction.
// Directions are either 0 (horizontal) or 1 (vertical).
int is_blockable(const struct board* b,
		 char column, char line,
		 char direction);

// Places a wall on the corresponding column, line and direction
// Directions are either 0 (horizontal) or 1 (vertical).
void place_wall(const struct board* b,
		char column, char line,
		char direction);

#endif //INTERFACE_H
