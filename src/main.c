/**
 * @file
 * @brief Server main engine
 */

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
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

int main(int argc, char* argv[])
{
    int i, playerStrategy[2], secondPlayer = 0;
    char line[LINE_SIZE];

    for (i = 0; i < strategies_count; i++) {
	printf("Strat n°%d : %s\n", i,
	       strategies[i]->name);
    }
    printf("\nNB of the black player's strat:\n");
    fgets(line, LINE_SIZE, stdin);
    playerStrategy[BLACK] = atoi(line);
    printf("NB of the white player's strat:\n");
    fgets(line, LINE_SIZE, stdin);
    playerStrategy[WHITE] = atoi(line);

    srand(time(NULL));
    gameRecord = fopen("game_record.txt", "w+");
    init_board();

    #ifdef USE_NCURSES
    initscr();
    #endif
    
    print_board();
    put_in_case(4, 0, "W");
    put_in_case(4, 8, "B");
    print_remaining_walls();
    sleep(1);

    while (!is_finished()) {
	strategies[playerStrategy[get_current_player(NULL)]]->play(NULL);
	if (secondPlayer)
	    fprintf(gameRecord, "\n");
        secondPlayer = !secondPlayer;
	switch_player();
	usleep(990000u);
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
