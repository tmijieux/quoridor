#include  <ncurses.h>
#include "ncurses.h"
#include "board.h"

int
main(void)
{
    initscr();
    print_board();
    put_in_case(4, 0, "W");
    put_in_case(4, 8, "B");


    print_wall(3,3,VERTICAL);
    getch();
    endwin();
    return 0;
}
