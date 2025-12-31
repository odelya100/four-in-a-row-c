#ifndef AI_H
#define AI_H

#include "game.h"

/*
    מודול AI:
   רמות קושי 3 :
    1) קל: בחירה אקראית מעמודות חוקיות
    2) בינוני: ניצחון במהלך אחד / חסימה במהלך אחד / אחרת העדפת מרכז או אקראי
    3) קשה: Minimax עם Alpha-Beta (עומק קטן כדי לרוץ מהר)
*/

int aiChooseMoveEasy(const char board[ROWS][COLS]); // מחזירה col בין 0..6, או -1 אם אין מהלכים
int aiChooseMoveMedium(const char board[ROWS][COLS], char aiDisc, char humanDisc);
int aiChooseMoveHard(const char board[ROWS][COLS], char aiDisc, char humanDisc);

#endif
