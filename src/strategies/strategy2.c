 /**
 * @file
 * @brief Strategy 3 : minimax
 */

#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>

#include <assert.h>

#include "minimax.h"
#include "interface.h"
#include "registry.h"
#include "list.h"
#include "char_stack.h"


/**
 *@brief return opposite direction i.e UP for DOWN
 * and LEFT for RIGHT
 */
inline static char
other_pawnMove_direction(char dir)
{
    return (3 - dir);
}

/**
 * @brief convert direction from local to server
 * @return direction seen by the server
 */
inline static char
server_pawnMove_direction(char dir)
{
    return (dir > 1) ? 5 - dir : dir;
}

/**
 * @brief returns the player goal line
 */
inline static char
goal_line(char player)
{
    return (COLUMN_LENGTH-1)*(player==WHITE);
}

/**
 * @brief minimum of two integers
 */
inline static int
min(int a, int b)
{
    return a < b ? a : b;
}

/**
 * @brief maximum of two integers
 */
inline static int
max(int a, int b)
{
    return a > b ? a : b;
}

/**
 * @brief minimum of two doubles
 */
inline static double
dmin(double a, double b)
{
    return a < b ? a : b;
}

/**
 * @brief 
 */
static void
switch_player_local(struct QuoridorBoard *board)
{
    board->player = 1 - board->player;
}


// This does not check whether the target is valid
// you have to do that yourself
static int
target_node(char col, char line, char dir)
{
    switch (dir) {
    case UP:
	-- line;
	break;
    case DOWN:
	++ line;
	break;
    case LEFT:
	-- col;
	break;
    case RIGHT:
	++ col;
	break;
    case LEFTLEFT:
	col -= 2;
	break;
    case UPUP:
	line -= 2;
	break;
    case RIGHTRIGHT:
	col += 2;
	break;
    case DOWNDOWN:
	line += 2;
	break;
    case UPLEFT:
	col --;
	line --;
	break;
    case UPRIGHT:
	col ++;
	line --;
	break;
    case DOWNLEFT:
	col --;
	line ++;
	break;
    case DOWNRIGHT:
	col ++;
	line ++;
	break;
    }
    return (col * LINE_LENGTH + line);
}

static void
set_edge_value(struct QuoridorBoard *board,
	       char col, char line,
	       char dir, char value)
{
    if (
	(dir == LEFT && col <= 0) ||
	(dir == RIGHT && col >= LINE_LENGTH - 1) ||
	(dir == UP && line <= 0) ||
	(dir == DOWN && line >= COLUMN_LENGTH - 1)
    )
	// out of range, just do nothing
	return;

    int node = target_node(col, line, dir);
    int targetCol = node / LINE_LENGTH;
    int targetLine = node % LINE_LENGTH;

    board->adjacencyMatrix
	[(col * LINE_LENGTH + line)]
	[(int)dir]
	= value;
    board->adjacencyMatrix
	[(targetCol * LINE_LENGTH + targetLine)]
	[(int)other_pawnMove_direction(dir)]
	= value;
}

static char
get_edge_value(struct QuoridorBoard *board,
	       char col, char line, char dir)
{
    return board->adjacencyMatrix
	[col * LINE_LENGTH + line]
	[(int)dir];
}


// do what the name tells it do
static void
move_pawn_local(struct QuoridorBoard *board,
		struct Move *move,
		int BackOrForth)
{
    switch (BackOrForth) {
    case FORTH:
	board->playerCol[(int)board->player] = move->dstCol;
	board->playerLine[(int)board->player] = move->dstLine;
	break;
    case BACK:
	board->playerCol[(int)board->player] = move->srcCol;
	board->playerLine[(int)board->player] = move->srcLine;
	break;
    }
}

static void
set_wall(struct QuoridorBoard *board,
	       struct Move *move, char value)
{
    char line = move->dstLine, col = move->dstCol;
    
    switch (move->dir) {
    case VERTICAL:
	set_edge_value(board,
		       col, line,
		       RIGHT,
		       value);
	set_edge_value(board,
		       col, line + 1, 
		       RIGHT,
		       value);
	break;
    case HORIZONTAL:
	set_edge_value(board,
		       col, line,
		       DOWN,
		       value);
	set_edge_value(board,
		       col + 1, line,
		       DOWN,
		       value);
	break;
    }
}

// do what the name tells it do
static void
place_wall_local(struct QuoridorBoard *board,
		 struct Move *move)
{
    set_wall(board, move, BLOCKING);
}

static void
remove_wall(struct QuoridorBoard *board,
	    struct Move *move)
{
    set_wall(board, move, PASSABLE);
}

//get in a child node of game tree (described by *move)
static void
do_move(struct QuoridorBoard *board, struct Move *move)
{
    if (move == NULL)
	return;

    switch (move->type) {
    case PAWN:
	move_pawn_local(board, move, FORTH);
	break;
    case WALL:
	place_wall_local(board, move);
	-- board->playerWalls[(int)board->player];
	
#ifdef DEBUG
	assert(board->playerWalls[(int)board->player] > -1);
	assert(board->playerWalls[(int)board->player] < 11);
#endif
	break;
    }

    // we have now entered the 'game node' (in the game tree)
    // and it is the turn of the other player to play
    switch_player_local(board);
}


// get out of a game tree node (back to parent)
static void
undo_move(struct QuoridorBoard *board, struct Move *move)
{
    if (move == NULL)
	return;
    
    // as we get out of a node, we give back the hand
    // to the caller player
    switch_player_local(board);

    switch (move->type) {
    case PAWN:
	move_pawn_local(board, move, BACK);
	break;
    case WALL:
	remove_wall(board, move);
	++ board->playerWalls[(int)board->player];

#ifdef DEBUG
	assert(board->playerWalls[(int)board->player] > -1);
	assert(board->playerWalls[(int)board->player] < 11);
#endif
	break;
    }
}

// get the board from the server
static void
get_board(const struct board *b, struct QuoridorBoard *board)
{
    board->player = get_current_player(b);
    board->maximizingPlayer = board->player;

    board->playerWalls[BLACK] = remaining_bridges(b, BLACK);
    board->playerWalls[WHITE] = remaining_bridges(b, WHITE);

    for (char line = 0; line < COLUMN_LENGTH; line++) {
	for (char col = 0; col < LINE_LENGTH; col++) {
	    int elem = get_position(b, col, line);
	    if (elem != EMPTY) {
		board->playerCol[elem] = col;
		board->playerLine[elem] = line;
	    }
	    for (int dir = 0; dir < 4; dir++) {
		if (is_passable(b, col, line,
				server_pawnMove_direction(dir))) {
		    set_edge_value(board,
				   col, line, dir,
				   PASSABLE);
		}
	    }
	}
    }
}

static int
is_empty(struct QuoridorBoard *board,
	 char col, char line)
{
    return !((col == board->playerCol[BLACK] &&
	      line == board->playerLine[BLACK]) ||
	     (col == board->playerCol[WHITE] &&
	      line == board->playerLine[WHITE]));
}

static int
is_valid_pawn_target(struct QuoridorBoard *board,
		     char col, char line, char dir)
{
 
    switch (dir) {
    case UP:
	return (line > 0 &&
		get_edge_value(board,
			       col, line, UP) == PASSABLE &&
		is_empty(board, col, line-1));
	break;
    case DOWN:
	return (line < COLUMN_LENGTH-1 &&
		get_edge_value(board,
			       col, line, DOWN) == PASSABLE &&
		is_empty(board, col, line+1));
	break;
    case LEFT:
	return (col > 0 &&
		get_edge_value(board,
			       col, line, LEFT) == PASSABLE &&
		is_empty(board, col-1, line));
	break;
    case RIGHT:
	return (col < LINE_LENGTH-1 &&
		get_edge_value(board,
			       col, line, RIGHT) == PASSABLE &&
		is_empty(board, col+1, line));
	break;
	
    case LEFTLEFT:
	return (col > 1 &&
		!is_empty(board, col-1, line) &&
		get_edge_value(board,
			       col, line,
			       LEFT) == PASSABLE &&
		get_edge_value(board,
			       col-1, line,
			       LEFT) == PASSABLE);
	break;
    case UPUP:
	return (line > 1 &&
		!is_empty(board, col, line-1) &&
		get_edge_value(board,
			       col, line,
			       UP) == PASSABLE &&
		get_edge_value(board,
			       col, line-1,
			       UP) == PASSABLE);
	break;
    case RIGHTRIGHT:
	return (col < LINE_LENGTH-2 &&
		!is_empty(board, col+1, line) &&
		get_edge_value(board,
			       col, line,
			       RIGHT) == PASSABLE &&
		get_edge_value(board,
			       col+1, line,
			       RIGHT) == PASSABLE);
	break;
    case DOWNDOWN:
	return (line < COLUMN_LENGTH-2 &&
		!is_empty(board, col, line+1) &&
		get_edge_value(board,
			       col, line,
			       DOWN) == PASSABLE &&
		get_edge_value(board,
			       col, line+1,
			       DOWN) == PASSABLE);
     break;
     
     
    case UPLEFT:
	return (line > 0 &&
		col > 0 &&
		((!is_empty(board, col, line-1) &&
		  get_edge_value(board,
				 col, line,
				 UP) == PASSABLE &&
		  get_edge_value(board,
				 col, line-1,
				 UP) == BLOCKING &&
		  get_edge_value(board,
				 col, line-1,
				 LEFT) == PASSABLE) ||
		 (!is_empty(board, col-1, line) &&
		  get_edge_value(board,
				 col, line,
				 LEFT) == PASSABLE &&
		  get_edge_value(board,
				 col-1, line,
				 LEFT) == BLOCKING &&
		  get_edge_value(board,
				 col-1, line,
				 UP) == PASSABLE )));
	break;
    case UPRIGHT:
	return (line > 0 &&
		col < LINE_LENGTH-1 &&
		((!is_empty(board, col, line-1) &&
		  get_edge_value(board,
				 col, line,
				 UP) == PASSABLE &&
		  get_edge_value(board,
				 col, line-1,
				 UP) == BLOCKING &&
		  get_edge_value(board,
				 col, line-1,
				 RIGHT) == PASSABLE) ||
		 (!is_empty(board, col+1, line) &&
		  get_edge_value(board,
				 col, line,
				 RIGHT) == PASSABLE &&
		  get_edge_value(board,
				 col+1, line,
				 RIGHT) == BLOCKING &&
		  get_edge_value(board,
				 col+1, line,
				 UP) == PASSABLE )));
	break;
    case DOWNLEFT:
	return (line < COLUMN_LENGTH-1 &&
		col > 0 &&
		((!is_empty(board, col-1, line) &&
		  get_edge_value(board,
				 col, line,
				 LEFT) == PASSABLE &&
		  get_edge_value(board,
				 col-1, line,
				 LEFT) == BLOCKING &&
		  get_edge_value(board,
				 col-1, line,
				 DOWN) == PASSABLE) ||
		 (!is_empty(board, col, line+1) &&
		  get_edge_value(board,
				 col, line,
				 DOWN) == PASSABLE &&
		  get_edge_value(board,
				 col, line+1,
				 DOWN) == BLOCKING &&
		  get_edge_value(board,
				 col, line+1,
				 LEFT) == PASSABLE )));
	break;
    case DOWNRIGHT:
	return (line < COLUMN_LENGTH-1 &&
		col < LINE_LENGTH-1 &&
		((!is_empty(board, col+1, line) &&
		  get_edge_value(board,
				 col, line,
				 RIGHT) == PASSABLE &&
		  get_edge_value(board,
				 col+1, line,
				 RIGHT) == BLOCKING &&
		  get_edge_value(board,
				 col+1, line,
				 DOWN) == PASSABLE) ||
		 (!is_empty(board, col, line+1) &&
		  get_edge_value(board,
				 col, line,
				 DOWN) == PASSABLE &&
		  get_edge_value(board,
				 col, line+1,
				 DOWN) == BLOCKING &&
		  get_edge_value(board,
				 col, line+1,
				 RIGHT) == PASSABLE )));
	break;
    }
    return 0;
}

static int
dijkstra_shorter_path_length(struct QuoridorBoard *board,
			     char srcCol, char srcLine,
			     char goalLine)
{
    int i;
    char visited[BOARD_SIZE] = { 0 };
    int distance[BOARD_SIZE];
    
    for (i = 0; i < BOARD_SIZE; i++)
	distance[i] = 100;
    
    char currentCol = srcCol;
    char currentLine = srcLine;
    if (srcLine == goalLine)
	return 0;
    // source Node is not distant from himself :)
    distance[srcCol * LINE_LENGTH + srcLine] = 0;

    int nextNode;
    do {
	int dis = distance[currentCol * LINE_LENGTH + currentLine];

	// Mark current Node as visited
	visited[currentCol * LINE_LENGTH + currentLine] = 1;

	// Say hello to each currentNode's unvisited neighbour
	for (int dir = 0; dir < 4; dir++) {
	    if (get_edge_value(board, currentCol, currentLine, dir)
		== PASSABLE) {
		int node = target_node(currentCol, currentLine, dir);
		int targetCol = node / LINE_LENGTH;
		int targetLine = node % LINE_LENGTH;

		// useless ?
		if (visited[targetCol * LINE_LENGTH + targetLine])
		    continue;
		
		int *p = &distance[targetCol * LINE_LENGTH + targetLine];

		// oh, did we reached our goal ?!
		// how far are we from start?
		if (targetLine == goalLine)
		    return dis+1;
		
		// Tell your neighbour its distance to the source node
		*p = min(*p, dis+1);
		if (*p > 80)
		    return -1;

	    }
	}

	// Find unvisited node with lowest distance:
	nextNode = -1;
	dis = INT_MAX;
	for (i = 0; i < BOARD_SIZE; i++) {
	    if (!visited[i] && distance[i] <= dis){
		dis = distance[i];
		nextNode = i;
	    }
	}
	// Make it current for next step
	currentCol = nextNode / LINE_LENGTH;
	currentLine = nextNode % LINE_LENGTH;
    } while (nextNode != -1);
#ifdef DIJK_DEBUG
    fprintf(stderr, "\r\nError DIJKSTRA\n\r");
    exit(EXIT_FAILURE);
#endif
    return -1;
    // should never be reached
}

//go see Mertens page 6 and 7
static double
minimax_evaluation_function(struct QuoridorBoard *board)
{
    int p = board->maximizingPlayer;
    char c = board->playerCol[p];
    char l1 = board->playerLine[p] ;
    int goalLine = goal_line(p);
    double dijkstraMaxPlayer
	= (double) dijkstra_shorter_path_length(board, c, l1,
						goalLine);

    l1 = goalLine - l1;
    
    p = 1 - p;
    c = board->playerCol[p];
    char l2 = board->playerLine[p];
    goalLine = goal_line(p);
    double dijkstraMinPlayer =
	(double)dijkstra_shorter_path_length(board, c, l2,
					     goalLine);
    l2 = goalLine - l2;

    //double lineDiff = l1 - l2;
    double dijkstraDiff = (dijkstraMinPlayer - dijkstraMaxPlayer);
    dijkstraMaxPlayer =	(1 / dijkstraMaxPlayer);
    double randVal = (((double)rand()) / RAND_MAX);
    
    return //(0.6 * l1)  +(0.601 * lineDiff)
	(14.4 * dijkstraDiff)
	+ (8.16*dijkstraMaxPlayer)
	+ (0.2*dijkstraMinPlayer)
	+ randVal;
}

#ifdef DIJK_DEBUG

#include <curses.h>
#include "../display.h"

void
debug_dijkstra(struct QuoridorBoard *board)
{
    for(char c = 0; c < LINE_LENGTH; c++) {
	for (char l = 0; l < COLUMN_LENGTH; l++) {
	    char s[10] = { 0 };
	    sprintf(s, "%d", dijkstra_shorter_path_length(board,
							  c, l, 0));
	    put_in_case(c, l, s);
	}
    }
    getch();
    getch();
}
#endif
    
static char
has_wall(struct QuoridorBoard *board,
	 char col, char line, char wallDir)
{

    if ((col < 0 && wallDir == VERTICAL) ||
	(col >= LINE_LENGTH - 1
	 && wallDir == VERTICAL) ||
	(line < 0 && wallDir == HORIZONTAL) ||
	(line >= COLUMN_LENGTH - 1
	 && wallDir == HORIZONTAL))
	return 1;

    switch (wallDir) {
    case VERTICAL:
	return 
	    ((get_edge_value(board,
			     col, line,
			     RIGHT) == BLOCKING)
	    &&
	     (get_edge_value(board,
			     col, line + 1,
			     RIGHT) == BLOCKING));
	break;
    case HORIZONTAL:
	return
	    ((get_edge_value(board,
			     col, line,
			     DOWN) == BLOCKING)
	    &&
	     (get_edge_value(board,
			     col + 1, line,
			     DOWN) == BLOCKING));
	break;
    }
    return 1;
}



static int
block_player_aux(struct QuoridorBoard *board,
		  char col, char line, char wallDir,
		  char player)
{
    // depth first search
    // see Mertens doc p.4


    char label[BOARD_SIZE] = { 0 };
    struct CharStack s;
    // a char stack
    // element are pushed and poped two by two
    struct Move wallMove = {
	.type = WALL,
	.dstCol = col,
	.dstLine = line,
	.dir = wallDir
    };
    char goalLine = goal_line(player);
	
    empty_stack(&s);
    do_move(board, &wallMove);
    push_stack(&s, board->playerCol[(int)player]);
    push_stack(&s, board->playerLine[(int)player]);

    while (!is_empty_stack(&s)) {
	line = pop_stack(&s);
	col = pop_stack(&s);
	if (!label[line * COLUMN_LENGTH + col]) {
	    label[line * COLUMN_LENGTH + col] = 1;
	    
	    for (int dir = 0; dir < 4; dir ++) {
		if (get_edge_value(board,
				   col, line,
				   dir) == PASSABLE) {
		    int node = target_node(col, line, dir);
		    int targetCol = node / LINE_LENGTH;
		    int targetLine = node % LINE_LENGTH;
		    if (label[targetLine * COLUMN_LENGTH + targetCol] == 1)
			continue;
		    if (targetLine == goalLine) {
			undo_move(board, &wallMove);
			return 0;
		    }
		    push_stack(&s, targetCol);
		    push_stack(&s, targetLine);
		}
	    }
	}
    }
    
    undo_move(board, &wallMove);
    return 1;
}

static int
block_player(struct QuoridorBoard *board,
	     char col, char line, char wallDir)
{
    return (block_player_aux(board, col, line, wallDir, BLACK) ||
	    block_player_aux(board, col, line, wallDir, WHITE));
}

static int
wall_intersect(struct QuoridorBoard *board,
	       char col, char line, char wallDir)
{
    return has_wall(board, col, line, 1 - wallDir);
}

static int
wall_overlap(struct QuoridorBoard *board,
	       char col, char line, char dir)
{
    return ((dir == HORIZONTAL &&
	     (has_wall(board, col + 1, line, HORIZONTAL) ||
	      has_wall(board, col - 1, line, HORIZONTAL))) ||
	    (dir == VERTICAL &&
	     (has_wall(board, col, line + 1, VERTICAL) ||
	      has_wall(board, col, line - 1, VERTICAL))));
}

static int
is_blockable_local(struct QuoridorBoard *board,
		   char col, char line, char dir)
{
    return
	(
	    !has_wall(board, col, line, dir) && 
	    !wall_overlap(board, col, line, dir) &&
	    !wall_intersect(board, col, line, dir) &&
	    !block_player(board, col, line, dir)
	);
}


static double
minimax3(struct QuoridorBoard *board,
	 struct Move *currentMove,
	 int depth,
	 int isMaximizingPlayer,
	 double alpha,
	 double beta,
	 struct Move *bestMove);

static void
set_pawn_move(struct Move *move, char col, char line, char dir)
{
    int node = target_node(col, line, dir);
    int targetCol = node / LINE_LENGTH;
    int targetLine = node % LINE_LENGTH;
    move->type = PAWN;
    move->dstCol = targetCol;
    move->dstLine = targetLine;
    move->srcCol = col;
    move->srcLine = line;
}

static double
compute_player_moves(struct QuoridorBoard *board,
		     int depth, int isMaximizingPlayer,
		     double alpha, double beta,
		     struct Move *bestMove)
		     
{
    char col = board->playerCol[(int)board->player];
    char line = board->playerLine[(int)board->player];


    for (int dir = 0; dir < 12; dir ++) {
	if (is_valid_pawn_target(board, col, line, dir)) {
	    struct Move move;
	    set_pawn_move(&move, col, line, dir);
	    double tmp = minimax3(board, &move, depth - 1,
				  !isMaximizingPlayer,
				  alpha, beta, NULL);
	    if (isMaximizingPlayer && tmp > alpha) {
		alpha = tmp;
		if (bestMove != NULL) {
		    *bestMove = move;
		    // Only in the root node minimax call
		    // memorize what is the best Move
		}
	    }
	    if (isMaximizingPlayer && beta <= alpha)
		return alpha;
	    
	    if (!isMaximizingPlayer && tmp < beta) {
		beta = tmp;
	    }
	    if (!isMaximizingPlayer && beta <= alpha)
		return beta;
	}
    }
    return isMaximizingPlayer ? alpha: beta;
}

static void
set_wall_move(struct Move *move,
	      char col, char line, char wallDir)
{
    move->type = WALL;
    move->dstCol = col;
    move->dstLine = line;
    move->dir = wallDir;
}

static double
compute_wall_moves_aux(struct QuoridorBoard *board,
		       int depth, int isMaximizingPlayer,
		       double alpha, double beta,
		       struct Move *bestMove)
		      
{
    for (char c = 0; c < LINE_LENGTH; c++) {
	for (char l = 0; l < COLUMN_LENGTH; l++) {
	    for (char wallDir = 0; wallDir < 2; wallDir++) {
		if (is_blockable_local(board, c, l, wallDir)) {
		    struct Move move;
		    set_wall_move(&move, c, l, wallDir);
		    double tmp = minimax3(board, &move, depth - 1,
					  !isMaximizingPlayer,
					  alpha, beta, NULL);
		    if (isMaximizingPlayer && tmp > alpha) {
			alpha = tmp;
			if (bestMove != NULL) {
			    *bestMove = move;
			    // Only in the root node minimax call
			    // memorize what is the best Move
			}
		    }
		    if (isMaximizingPlayer && beta <= alpha)
			return alpha;
		    
		    if (!isMaximizingPlayer && tmp < beta) {
			beta = tmp;
		    }
		    if (!isMaximizingPlayer && beta <= alpha)
			return beta;
		}
	    }
	}
    }
    return isMaximizingPlayer ? alpha: beta;
}

static double
compute_wall_moves(struct QuoridorBoard *board,
		   int depth, int isMaximizingPlayer,
		   double alpha, double beta,
		   struct Move *bestMove)

{
    if (board->playerWalls[(int)board->player] > 0) {
	return compute_wall_moves_aux(board, depth,
				      isMaximizingPlayer,
				      alpha, beta, bestMove);
    }
#ifdef DIJK_DEBUG
    else {

	debug_dijkstra(board);
    }
#endif
    return isMaximizingPlayer ? alpha: beta;
}

/**
 * @brief minimax implementation
 * @param[in] QuoridorBoard
 * @param[in] currentMove
 * @param[in] isMaximizingPlayer
 * @param[out] bestMove return value for choosing the best move
 * @return the value of the node('currentMove')
 */
static double
minimax3(struct QuoridorBoard *board,
	 struct Move *currentMove,
	 int depth,
	 int isMaximizingPlayer,
	 double alpha,
	 double beta,
	 struct Move *bestMove)
{
    //modify board according to currentMove    
    do_move(board, currentMove);
    if (board->playerLine[(int)board->maximizingPlayer] ==
	goal_line(board->maximizingPlayer)) {
	undo_move(board, currentMove);
	return 99998.;
    }

    if (depth == 0) {
	double value = minimax_evaluation_function(board);
	undo_move(board, currentMove);
	return value;
    }

    //struct List *childMoveList = empty_list();
    //compute_player_moves(board, childMoveList);
    //compute_wall_moves(board, childMoveList);

    double ret;
    //struct Move *childMove = NULL;

    // the actual max and min:    
    if (isMaximizingPlayer) {
	alpha = compute_player_moves(board, depth,
				     isMaximizingPlayer,
				     alpha, beta, bestMove);
	alpha = compute_wall_moves(board, depth,
				   isMaximizingPlayer,
				   alpha, beta, bestMove);
	ret = alpha;

    } else {
	beta = compute_player_moves(board, depth,
				    isMaximizingPlayer,
				    alpha, beta, bestMove);
	beta = compute_wall_moves(board, depth,
				  isMaximizingPlayer,
				  alpha, beta, bestMove);
	ret = beta;
    }

    //destruct_list(childMoveList);
    undo_move(board, currentMove);

    return ret;
}

/**
 * @brief minimax strategy
 */
void
strategy3__play(const struct board *b)
{
    // MINIMAX what we need to to :
    // for a given player: compute the square
    // where he is allowed to go
    // i.e the 'childs' in the minimax algorithm
    // walls he can place
    // - a heuristic value for the leaves: the length
    // of the shorter path to the win lane by example
    // when it is done we should try various depth for the
    // algorithm  and see how fast it is

    struct QuoridorBoard board = { .adjacencyMatrix = { { BLOCKING } } };
    struct Move bestMove;

    get_board(b, &board);
    minimax3(&board, NULL, MINIMAX_DEPTH, 1,
	     -99999., 99999., &bestMove);
    switch (bestMove.type) {
    case PAWN:
	move_pawn(b, bestMove.dstCol, bestMove.dstLine);
	break;
    case WALL:
	place_wall(b,
		   bestMove.dstCol,
		   bestMove.dstLine,
		   bestMove.dir);
	break;
    default:
	fprintf(stderr, "error selecting a move\n");
	exit(EXIT_FAILURE);
	break;
    }
}

REGISTER_STRATEGY("Minimax 2", strategy3__play);
