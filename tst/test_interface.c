#include <stdio.h>
#include <assert.h>

#include "../src/interface.h"
#include "../src/board.h"
#include "../src/display.h"
#include "../src/server.h"

#include <time.h>
#include <stdlib.h>

extern struct board bo;
FILE *gameRecord;

#undef USE_NCURSES
#undef PRINT_IN_FILE

// Tests on server functions
void test_init_board();
void test_is_finished();
void test_switch_player();
// Tests on interface functions
void test_get_current_player();
void test_get_position();
void test_is_passable();
void test_move_pawn();
void test_remaining_bridges();
void test_is_blockable();
void test_place_wall();
// Tests on private functions
void test_block_player();
void test_is_goal();
void test_wall_intersect();
void test_wall_overlap();
void test_has_wall();

int main(int argc, char **argv)
{
    srand(time(NULL));

    printf("===================================================================\n");
    printf("\t\tTESTING INTERFACE.C\n");
    printf("===================================================================\n\n");
    
    printf("\tTests on server functions\n");
    printf("___________________________________________________________________\n");
    /* 
     * we test init_board() first, so that we
     * can use it in the others test functions
     */
    printf("Testing init_board() ......................................");
    test_init_board();
    printf("\tOK\n");
    printf("Testing is_finished() .....................................");
    test_is_finished();
    printf("\tOK\n");

    printf("Testing switch_player() ...................................");
    test_switch_player();
    printf("\tOK\n");
    printf("\n");

    printf("\tTests on interface functions\n");
    printf("___________________________________________________________________\n");
    printf("Testing get_current_player() ..............................");
    test_get_current_player();
    printf("\tOK\n");
    printf("Testing test_get_position() ...............................");
    test_get_position();
    printf("\tOK\n");
    printf("Testing is_passable() .....................................");
    test_is_passable();
    printf("\tOK\n");
    printf("Testing move_pawn() .......................................");
    test_move_pawn();
    printf("\tOK\n");
    printf("Testing remaining_bridges() ...............................");
    test_remaining_bridges();
    printf("\tOK\n");
    printf("Testing is_blockable() ....................................");
    test_is_blockable();
    printf("\tOK\n");
    printf("Testing place_wall() ......................................");
    test_place_wall();
    printf("\tOK\n");
    printf("\n");

    printf("\tTests on private functions\n");
    printf("___________________________________________________________________\n");
    printf("Testing block_player() ....................................");
    test_block_player();
    printf("\tOK\n");
    printf("Testing is_goal() .........................................");
    test_is_goal();
    printf("\tOK\n");
    printf("Testing wall_intersect() ..................................");
    test_wall_intersect();
    printf("\tOK\n");
    printf("Testing wall_overlap() ....................................");
    test_wall_overlap();
    printf("\tOK\n");
    printf("Testing has_wall() ........................................");
    test_has_wall();
    printf("\tOK\n");

    return 0;
}

/* ---------- Tests on server functions ---------- */

void test_init_board()
{
    init_board();

    assert(bo.nbWalls == 0);
    assert(bo.nbWallsPlayer[BLACK] == 0);
    assert(bo.nbWallsPlayer[WHITE] == 0);

    assert(bo.players[BLACK].col == E);
    assert(bo.players[BLACK].line == 8);
    
    assert(bo.players[WHITE].line == 0);
    assert(bo.players[WHITE].col == E);
    
    assert(bo.currentPlayer == BLACK || bo.currentPlayer == WHITE);
    assert(bo.lastPlayer == 1 - bo.currentPlayer);
}

void test_is_finished()
{
    init_board();
    
    // at the beginning, the game is not finished
    assert(is_finished() == 0);

    // placing the BLACK player on the goal line
    bo.players[BLACK].col = C;
    bo.players[BLACK].line = 0;
    assert(is_finished() == 1);
}

void test_switch_player()
{
    init_board();
    if (bo.currentPlayer == WHITE)
    {
	switch_player();
    }
    assert(bo.currentPlayer == BLACK);
}

/* ---------- Tests on interface functions ---------- */

void test_get_current_player()
{
    init_board();

    bo.currentPlayer = WHITE;
    assert(get_current_player(&bo) == WHITE);

    bo.currentPlayer = BLACK;
    assert(get_current_player(&bo) == BLACK);
}

void test_get_position()
{
    init_board();
    
    assert(get_position(&bo, G, 4) == EMPTY);

    bo.players[WHITE].col = C;
    bo.players[WHITE].line = 5; 
    assert(get_position(&bo, C, 5) == WHITE);

    bo.players[BLACK].col = D;
    bo.players[BLACK].line = 6;
    assert(get_position(&bo, D, 6) == BLACK);
}

void test_is_passable()
{
    init_board();

    assert(is_passable(&bo, D, 3, UP) == 1);

    bo.walls[bo.nbWalls].col = F;
    bo.walls[bo.nbWalls].line = 5;
    bo.walls[bo.nbWalls].dir = VERTICAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    assert(is_passable(&bo, F, 5, RIGHT) == 0);
}

void test_move_pawn()
{
    /* destination is far from the player */
    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = E;
    bo.players[BLACK].line = 5;
    move_pawn(&bo, H, 5); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 5);
    
    /* out of the board */
    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = A;
    bo.players[BLACK].line = 0;
    move_pawn(&bo, A, -1); // forbidden
    assert(bo.players[BLACK].col == A &&
	   bo.players[BLACK].line == 0);

    /* destination square is not empty */
    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = D;
    bo.players[BLACK].line = 4;
    // WHITE player is just at the right of BLACK
    // so this square is not empty
    bo.players[WHITE].col = E;
    bo.players[WHITE].line = 4;
    move_pawn(&bo, E, 4);
    // it's a forbidden move, his position doesn't change
    assert(bo.players[BLACK].col == D &&
	   bo.players[BLACK].line == 4);
    
    /* simple case */
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    move_pawn(&bo, F, 0);
    assert(bo.players[WHITE].col == F &&
	   bo.players[WHITE].line == 0);

    /* jump with no obstacle case */
    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = G;
    bo.players[BLACK].line = 6;
    // WHITE player is just 'above' BLACK
    bo.players[WHITE].col = G;
    bo.players[WHITE].line = 5;
    // so BLACK can jump over him
    move_pawn(&bo, G, 4);
    assert(bo.players[BLACK].col == G &&
	   bo.players[BLACK].line == 4);

    /* jump with obstacle cases */
    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = G;
    bo.players[BLACK].line = 6;
    // WHITE player is just 'above' BLACK
    bo.players[WHITE].col = G;
    bo.players[WHITE].line = 5;
    // but there's a wall above him
    bo.walls[bo.nbWalls].col = G;
    bo.walls[bo.nbWalls].line = 4;
    bo.walls[bo.nbWalls].dir = HORIZONTAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    // so BLACK can't go in the square that is above WHITE
    move_pawn(&bo, G, 4);
    // his position doesn't change
    assert(bo.players[BLACK].col == G &&
	   bo.players[BLACK].line == 6);    
    // but he can go next to WHITE
    move_pawn(&bo, H, 5);
    assert(bo.players[BLACK].col == H &&
	   bo.players[BLACK].line == 5);

    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[BLACK].col = E;
    bo.players[BLACK].line = 6;
    // WHITE player is just above him
    bo.players[WHITE].col = E;
    bo.players[WHITE].line = 5;
    // there's a wall at the right
    bo.walls[bo.nbWalls].col = E;
    bo.walls[bo.nbWalls].line = 5;
    bo.walls[bo.nbWalls].dir = VERTICAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    // there's a wall above WHITE player
    bo.walls[bo.nbWalls].col = E;
    bo.walls[bo.nbWalls].line = 4;
    bo.walls[bo.nbWalls].dir = HORIZONTAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    // and there's a wall at the left of WHITE
    // player but not of BLACK player
    bo.walls[bo.nbWalls].col = D;
    bo.walls[bo.nbWalls].line = 4;
    bo.walls[bo.nbWalls].dir = VERTICAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    // so, the only possibilities for BLACK
    // are (D, 6) and (E, 7), all the others
    // moves are forbidden (and so, his position
    // doesn't change
    move_pawn(&bo, E, 5); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 6);
    move_pawn(&bo, F, 5); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 6);
    move_pawn(&bo, D, 5); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 6);
    move_pawn(&bo, E, 4); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 6);
    move_pawn(&bo, F, 6); // forbidden
    assert(bo.players[BLACK].col == E &&
	   bo.players[BLACK].line == 6);
    move_pawn(&bo, D, 6); // authorized
    assert(bo.players[BLACK].col == D &&
	   bo.players[BLACK].line == 6);

    init_board();
    bo.lastPlayer = WHITE;
    bo.currentPlayer = BLACK;
    bo.players[WHITE].col = A;
    bo.players[WHITE].line = 3;
    bo.players[BLACK].col = B;
    bo.players[BLACK].line = 3;
    bo.walls[bo.nbWalls].col = B;
    bo.walls[bo.nbWalls].line = 2;
    bo.walls[bo.nbWalls].dir = HORIZONTAL;
    ++ bo.nbWalls;
    ++ bo.nbWallsPlayer[WHITE];
    assert(is_passable(NULL, A, 3, LEFT) == 0);
    assert(is_passable(NULL, A, 2, DOWN) == 1);
    assert(is_passable(NULL, A, 3, UP) == 1);
    move_pawn(NULL, A, 2);
    assert(bo.players[BLACK].col == A &&
	   bo.players[BLACK].line == 2);
    
    /* a player try to play twice */
    init_board();
    bo.lastPlayer = WHITE;
    // the current player is the same
    bo.currentPlayer = WHITE;
    bo.players[WHITE].col = F;
    bo.players[WHITE].line = 3;
    move_pawn(&bo, G, 3);
    assert(bo.players[WHITE].col == F &&
	   bo.players[WHITE].line == 3);
}

void test_remaining_bridges()
{
    init_board();
    bo.nbWalls = 5;
    bo.nbWallsPlayer[BLACK] = 2;
    bo.nbWallsPlayer[WHITE] = 3;
    int nbWallsRemainingBlack = MAX_WALLS_PER_PLAYER - 2;
    int nbWallsRemainingWhite = MAX_WALLS_PER_PLAYER - 3;
    assert(remaining_bridges(&bo, BLACK) == nbWallsRemainingBlack);
    assert(remaining_bridges(&bo, WHITE) == nbWallsRemainingWhite);
}

void test_is_blockable()
{
    init_board();
    assert(is_blockable(&bo, C, 5, VERTICAL) == 1);
    bo.walls[bo.nbWalls].col = C;
    bo.walls[bo.nbWalls].line = 5;
    bo.walls[bo.nbWalls].dir = VERTICAL;
    bo.nbWalls++;
    bo.nbWallsPlayer[WHITE]++;
    assert(is_blockable(&bo, C, 5, VERTICAL) == 0);

    // TODO: add many other tests
}

void test_place_wall()
{
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo, F, 5, HORIZONTAL);
    assert(bo.nbWallsPlayer[WHITE] == 1);
    assert(bo.nbWalls == 1);
    assert(bo.walls[0].col == F &&
	   bo.walls[0].line == 5 &&
	   bo.walls[0].dir == HORIZONTAL);
    
    // TODO: add many other tests
}

/* ---------- Tests on private functions ---------- */

int block_player(const struct board *b,
		 char column, char line,
		 char direction);

 int is_goal(char line, int player);


 int block_player__(const struct board *b,
		    char column, char line,
		    char direction, int player);

 int block_player__R(const struct board *b,
		     char column, char line,
		     int player, int mem[]);

 int wall_intersect(char column, char line,
		    char direction);

 int wall_overlap(char column, char line,
		  char direction);

 int has_wall(char column, char line,
	      char direction);

 int is_next_to_opponent(int direction);

void test_block_player()
{
    // no blocking case
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    char line = 2;
    char col = B;
    char dir = HORIZONTAL;
    line = 3;
    col = B;
    dir = HORIZONTAL;
    assert (!block_player(&bo,col,line,dir));
  
    // blocking case
    init_board(); 
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo,0,1,HORIZONTAL);

    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE; 
    place_wall(&bo,2,1,HORIZONTAL);

    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo,4,1,HORIZONTAL);

    assert(block_player(&bo,5,0,VERTICAL)==1);
}

void test_is_goal()
{
    char line = 0;
    int player = BLACK;
    assert (is_goal(line,player));
    line = BOARD_SIZE-1;
    player=WHITE;
    assert (is_goal(line,player));
}

void test_wall_intersect()
{
    init_board();
    char line = 5;
    char col = 2;
    char dir = 0;
    assert (!wall_intersect(col,line,dir));
    place_wall(&bo,col,line,1-dir);
    assert (wall_intersect(col,line,dir));
}

void test_wall_overlap()
{
    // horizontal case
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo, F, 3, VERTICAL);
    assert(wall_overlap(F, 2, VERTICAL));
    assert(wall_overlap(F, 4, VERTICAL));
    // vertical case
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo, F, 3, HORIZONTAL);
    assert(wall_overlap(E, 3, HORIZONTAL ));
    assert(wall_overlap(G, 3, HORIZONTAL));
  
}

void test_has_wall()
{
    // boarders of the board
    assert (has_wall(-1,3,VERTICAL));
    // inside the board
    init_board();
    bo.lastPlayer = BLACK;
    bo.currentPlayer = WHITE;
    place_wall(&bo, F, 5, HORIZONTAL);
    assert(has_wall(F, 5, HORIZONTAL));

}
