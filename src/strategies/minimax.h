#ifndef MINIMAX_H
#define MINIMAX_H

#define LINE_LENGTH 9
#define COLUMN_LENGTH 9
#define BOARD_SIZE ((LINE_LENGTH) * (COLUMN_LENGTH))

#define MINIMAX_DEPTH 3

enum {
    HORIZONTAL = 0,
    VERTICAL = 1
};

enum {
    BACK,
    FORTH
};

enum {
    WALL,
    PAWN
};

enum {
    BLOCKING = 0,
    PASSABLE = 1
};

enum {
    EMPTY = -1,
    WHITE = 0,
    BLACK = 1
};


#ifndef STRAT__2

enum {
    
    LEFT = 0,
    UP,
    DOWN,
    RIGHT,
    DOWNDOWN,
    UPUP,
    RIGHTRIGHT,
    LEFTLEFT,
    UPLEFT,
    DOWNLEFT,
    UPRIGHT,
    DOWNRIGHT
};

struct QuoridorBoard {
    char adjacencyMatrix[BOARD_SIZE][4];

    char maximizingPlayer;
    char player;
    char playerCol[2];
    char playerLine[2];
    char playerWalls[2];
};

#endif

struct Move {
    char type;
    char srcCol;
    char srcLine;

    char dstCol;
    char dstLine;

    char dir;
};


#endif //MINIMAX_H
