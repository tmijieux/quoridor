/**
 * @file
 * @brief Definition if what is a board
 * and various constants
 */

#ifndef BOARD_H
#define BOARD_H

#define BOARD_SIZE 9

#define EMPTY -1
#define WHITE 0
#define BLACK 1

#define MAX_WALLS_PER_PLAYER (10)
#define MAX_WALLS (MAX_WALLS_PER_PLAYER * 2)

enum {
    LEFT = 0,
    UP,
    RIGHT,
    DOWN,
    DOWNDOWN,
    UPUP,
    RIGHTRIGHT,
    LEFTLEFT,
    UPLEFT,
    DOWNLEFT,
    UPRIGHT,
    DOWNRIGHT
};

enum {
    HORIZONTAL = 0,
    VERTICAL
};

enum {
    A = 0, 
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I
};

struct player {
    char col;
    char line;
};

struct wall {
    char col;
    char line;
    char dir;
};

struct board {
    struct player players[2];
    int currentPlayer;
    int lastPlayer;
	
    struct wall walls[MAX_WALLS + 1];
    int nbWalls;
    int nbWallsPlayer[2];
};

#endif //BOARD_H
