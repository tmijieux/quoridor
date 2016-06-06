/**
 * @file
 * @brief Graphic output functions
 */

#include <ncurses.h>
#include "board.h"
#include "interface.h"

#define LEFT_MARGIN 5
#define TOP_MARGIN 0

/**
 * @brief Put the content of str in a square of the board
 * @param[in] column the column of the square
 * @param[in] line the line of the square
 * @param[in] str the str you want to output
 */
void put_in_case(char column, char line,
		 const char *str)
{
#ifdef USE_NCURSES
    mvprintw(TOP_MARGIN + 4 + (2*line),
	     LEFT_MARGIN + 2 + (4*column),
	     str);
    mvprintw(TOP_MARGIN + 21,
	     LEFT_MARGIN + 42,
	     " ");
    refresh();
#endif
}

/**
 * @brief Print the wall at the right place
 * @param[in] column the column of the wall
 * @param[in] line the line of the wall
 * @param[in] direction the direction of the wall
 */
void print_wall(char column, char line, char direction)
{
#ifdef USE_NCURSES
    switch (direction) {
    case HORIZONTAL:
	mvprintw(TOP_MARGIN + 5 +(2*line),
		 LEFT_MARGIN + 1 +(4*column), "---");
	
	mvprintw(TOP_MARGIN + 5 +(2*line),
		 LEFT_MARGIN + 5 + (4*column), "---");
	break;
    case VERTICAL:
	mvprintw(TOP_MARGIN + 4 + (2*line),
		 LEFT_MARGIN + 4 + (4*column), "|");
	mvprintw(TOP_MARGIN + 6 +(2*line),
		 LEFT_MARGIN + 4 + (4*column), "|");
	break;
    }
    mvprintw(TOP_MARGIN + 21, LEFT_MARGIN + 42, " ");
    refresh();
#endif
}

/**
 * @brief Print the available walls the both players have
 */
void print_remaining_walls(void)
{
#ifdef USE_NCURSES
    int i, k = remaining_bridges(NULL, WHITE);
    
    for(i = 0; i < k; i++) {
	mvprintw(TOP_MARGIN,
		 LEFT_MARGIN + (4*i), "|");
	mvprintw(TOP_MARGIN + 1,
		 LEFT_MARGIN + (4*i), "|");

    }
    while (i < MAX_WALLS_PER_PLAYER) {
	mvprintw(TOP_MARGIN,
		 LEFT_MARGIN + (4*i), " ");
	mvprintw(TOP_MARGIN + 1,
		 LEFT_MARGIN + (4*i), " ");
	i++;
    }

    k = remaining_bridges(NULL, BLACK);
    for(i = 0; i < k; i++) {
	mvprintw(TOP_MARGIN + 23,
		 LEFT_MARGIN +(4*i), "|");
	mvprintw(TOP_MARGIN + 24,
		 LEFT_MARGIN +(4*i), "|");
    }
    while (i < MAX_WALLS_PER_PLAYER) {
	mvprintw(TOP_MARGIN + 23,
		 LEFT_MARGIN +(4*i), " ");
	mvprintw(TOP_MARGIN + 24,
		 LEFT_MARGIN +(4*i), " ");
	i++;
    }
    mvprintw(TOP_MARGIN + 21, LEFT_MARGIN + 42, " ");
    refresh();
#endif
}

/**
 * @brief Print the empty board layer
 */
void print_board(void)
{
#ifdef USE_NCURSES
    int i;
    mvprintw(TOP_MARGIN + 3,
	     LEFT_MARGIN,
	     "+---+---+---+---+---+---+---+---+---+");
    
    mvprintw(TOP_MARGIN+4,
	     LEFT_MARGIN,
	     "|                                   |");
    
    for(i = TOP_MARGIN+5; i <  TOP_MARGIN + 21; i+=2) {
	mvprintw(i, LEFT_MARGIN, "+   +   +   +   +   +   +   +   +   +");
	mvprintw(i+1, LEFT_MARGIN, "|                                   |");
	
    }
    
    mvprintw(TOP_MARGIN + 21,
	     LEFT_MARGIN,
	     "+---+---+---+---+---+---+---+---+---+");
    mvprintw(TOP_MARGIN + 21, LEFT_MARGIN + 42, " ");
    refresh();
#endif
}



#ifdef TEST
extern struct board bo;

void
print_walls(void)
{
    for (int i = 0; i < bo.nbWalls; i++) {
	print_wall(bo.walls[i].col, bo.walls[i].line,
		   bo.walls[i].dir);
    }
}


void
force_whole_board_display()
{
#ifdef USE_NCURSES
    print_board();
    print_remaining_walls();
    print_walls();
    put_in_case(bo.players[WHITE].col, bo.players[WHITE].line
		, "W");
    put_in_case(bo.players[BLACK].col, bo.players[BLACK].line
		, "B");
#endif // USE_NCURSES
}

#endif // TEST
