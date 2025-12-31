#ifndef GAME_H
#define GAME_H

#define ROWS 6
#define COLS 7
#define EMPTY '.'

void initBoard(char board[ROWS][COLS]);
void printBoard(const char board[ROWS][COLS]);
int  isValidColumn(const char board[ROWS][COLS], int col);     // col: 0..6
int  dropDisc(char board[ROWS][COLS], int col, char disc);     // returns row or -1
int  checkWin(const char board[ROWS][COLS], char disc);        // 1 if win
int  isBoardFull(const char board[ROWS][COLS]);                // 1 if full

#endif

