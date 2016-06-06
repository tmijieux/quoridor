/**
 * @file
 * @brief Server main engine
 */

#define _DEFAULT_SOURCE
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <time.h>

#include "interface.h"
#include "server.h"

#include "registry.h"
#include "board.h"
#include "display.h"

#define LINE_SIZE 12

FILE *gameRecord;

void
init_board_debug(int nbWalls,
		 char currentPlayer, char lastPlayer,
		 char blackLine, char blackCol,
		 char whiteLine, char whiteCol);

void
place_wall_debug(char col, char line, char dir);

int
main(int argc, char* argv[])
{
    int playerStrategy[2], secondPlayer = 0;

    srand(time(NULL));
    playerStrategy[BLACK] = 0;
    playerStrategy[WHITE] = 0; // RANDOM
    gameRecord = fopen("game_record.txt", "w+");


    // WHITE IS RANDOM AND BLOCKED BY BLACK
    
    init_board_debug(3, WHITE, BLACK,
		     3, 5,
		     3, 4);
    place_wall_debug(2, 5, VERTICAL);
    place_wall_debug(3, 4, VERTICAL);
    place_wall_debug(3, 5, HORIZONTAL);


    #ifdef USE_NCURSES
    initscr();
    force_whole_board_display();
    #endif
    
    sleep(1);

    while (!is_finished()) {
	strategies[playerStrategy[get_current_player(NULL)]]->play(NULL);
	if (secondPlayer)
	    fprintf(gameRecord, "\n");
        secondPlayer = !secondPlayer;
	switch_player();
	sleep(1);
    }

    if (secondPlayer)
	fprintf(gameRecord, "\n");
    fclose(gameRecord);
    
    #ifdef USE_NCURSES
    getch();
    endwin();
    #endif
    
    return EXIT_SUCCESS;
}
