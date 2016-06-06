/**
 * @file
 * @brief Output functions headers
 */

#ifndef DISPLAY_H
#define DISPLAY_H

void put_in_case(char column, char line,
		 const char *str);

void print_wall(char column, char line,
		char direction);

void print_board(void);

void print_remaining_walls(void);

#ifdef TEST
void force_whole_board_display();
#endif

#endif //DISPLAY_H
