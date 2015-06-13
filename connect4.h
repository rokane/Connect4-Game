/* 
   Header File for connect4.c
   Written by Ryan O'Kane: rokane - 587723
   COMP20023 - Computer Systems: Project 2A
*/

#ifndef CONNECT4_H
#define CONNECT4_H

/* Game Constants */
#define WIDTH    7
#define HEIGHT   6
#define STRAIGHT 4

#define EMPTY   ' '
#define RED	    'R'
#define YELLOW  'Y'

#define WGRID    5
#define HGRID    3

#define RSEED 876545678

/* Game Board Data Structure */
typedef char c4_t[HEIGHT][WIDTH];


/* Function Declarations */
void print_config(c4_t board);
void init_empty(c4_t board);
int do_move(c4_t board, int col, char colour);
void undo_move(c4_t board, int col);
int get_move(c4_t board);
int move_possible(c4_t board);
char winner_found(c4_t board);
int rowformed(c4_t board,  int r, int c);
int explore(c4_t board, int r_fix, int c_fix, int r_off, int c_off);
int suggest_move(c4_t board, char colour);

#endif