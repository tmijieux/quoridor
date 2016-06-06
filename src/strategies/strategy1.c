/**
 * @file
 * @brief Strategy 1 : random bot
 */

#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "registry.h"

enum {
    LEFT = 0,
    UP,
    RIGHT,
    DOWN
};

#define EMPTY -1
#define WHITE 0
#define BLACK 1

void strategy1__play(const struct board *b)
{
    int player, playerCol = -1, playerLine;
    char line, column, direction;
    int randomValue;

    player = get_current_player(b);
    for (line = 0; line < 9; line++) {
	for (column = 0; column < 9; column++) {
	    if (get_position(b, column, line) == player) {
		playerLine = line;
		playerCol = column;
		break;
	    }
	}
	if (playerCol>0)
		break;
    }

    randomValue = rand() % 100;
    if (randomValue > 75) {
	if (remaining_bridges(b, player) > 0) {
	    do {
		line = rand() % 9;
		column = rand() % 9;
		direction = rand() % 2;
	    } while (!is_blockable(b, column, line, direction));
	    place_wall(b, column, line, direction);
	    return;
	}
    }

    do {
	direction = rand() % 4;
	switch (direction) {
	case LEFT:
	    line = playerLine;
	    column = playerCol-1;
	    break;
	case UP:
	    line = playerLine-1;
	    column = playerCol;
	    break;
	case RIGHT:
	    line = playerLine;
	    column = playerCol+1;
	    break;
	case DOWN:
	    line = playerLine + 1;
	    column = playerCol;
	    break;
	}
    } while (!(is_passable(b, playerCol, playerLine, direction) &&
	       get_position(b, column, line) == EMPTY));

    move_pawn(b, column, line);
};

REGISTER_STRATEGY("Random", strategy1__play);
