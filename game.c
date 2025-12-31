#include <stdio.h>
#include "game.h"

/*
    פונקציית עזר פנימית (static):
    בודקת האם אינדקסים נמצאים בגבולות הלוח.
    static => לא נחשפת מחוץ לקובץ.
*/
static int inBounds(int r, int c) {
    return (r >= 0 && r < ROWS && c >= 0 && c < COLS);
}

void initBoard(char board[ROWS][COLS]) {
    // איפוס מלא: כל תאים לריק
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            board[r][c] = EMPTY;
        }
    }
}

void printBoard(const char board[ROWS][COLS]) {
    /*
        הדפסה "ידידותית":
        שורה עליונה: מספרי עמודות 1..7 כדי שהמשתמש יבחר בקלות.
        לאחר מכן שורות הלוח.
    */
    printf("\n  ");
    for (int c = 0; c < COLS; c++) printf("%d ", c + 1);
    printf("\n");

    for (int r = 0; r < ROWS; r++) {
        printf("| ");
        for (int c = 0; c < COLS; c++) {
            printf("%c ", board[r][c]);
        }
        printf("|\n");
    }

    printf("  ");
    for (int c = 0; c < COLS; c++) printf("--");
    printf("\n");
}

int isValidColumn(const char board[ROWS][COLS], int col) {
    /*
        עמודה חוקית = בטווח + התא העליון ריק.
        למה התא העליון? כי אם התא העליון תפוס - העמודה מלאה (אין לאן "ליפול").
    */
    if (col < 0 || col >= COLS) return 0;
    return board[0][col] == EMPTY;
}

int dropDisc(char board[ROWS][COLS], int col, char disc) {
    /*
        הפלת דיסק:
        מתחילים מהשורה התחתונה (ROWS-1) ועולים למעלה עד שמוצאים תא ריק.
        זה מחקה "כבידה" כמו במשחק האמיתי.
    */
    if (!isValidColumn(board, col)) return -1;

    for (int r = ROWS - 1; r >= 0; r--) {
        if (board[r][col] == EMPTY) {
            board[r][col] = disc;
            return r;
        }
    }
    // תיאורטית לא נגיע לכאן בגלל isValidColumn, אבל נשאיר ליתר ביטחון
    return -1;
}

int isBoardFull(const char board[ROWS][COLS]) {
    /*
        לוח מלא = אין עמודה חוקית => כל התאים העליונים תפוסים.
        זה בדיקה O(COLS) במקום לסרוק את כל הלוח.
    */
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] == EMPTY) return 0;
    }
    return 1;
}

int checkWin(const char board[ROWS][COLS], char disc) {
    /*
        בדיקת ניצחון:
        עבור כל תא שיש בו את הדיסק, נבדוק 4 כיוונים אפשריים:
        1) אופקי ימינה
        2) אנכי למטה
        3) אלכסון יורד-ימין
        4) אלכסון יורד-שמאל

        אם מצאנו 4 רצופים באחד הכיוונים -> ניצחון.
    */
    const int dr[4] = {0, 1, 1, 1};
    const int dc[4] = {1, 0, 1, -1};

    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
            if (board[r][c] != disc) continue;

            for (int k = 0; k < 4; k++) {
                int count = 1;
                int rr = r, cc = c;

                // נבדוק עוד 3 צעדים קדימה כדי להגיע ל-4 ברצף
                for (int step = 1; step < 4; step++) {
                    rr += dr[k];
                    cc += dc[k];
                    if (!inBounds(rr, cc)) break;
                    if (board[rr][cc] != disc) break;
                    count++;
                }

                if (count == 4) return 1;
            }
        }
    }
    return 0;
}
