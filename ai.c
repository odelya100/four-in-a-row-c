#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "ai.h"

/*
    פונקציית עזר:
    אוספת את כל העמודות החוקיות (שהתא העליון שלהן ריק) לתוך מערך cols.
    מחזירה כמה עמודות חוקיות יש.
*/
static int validColumns(const char board[ROWS][COLS], int cols[COLS]) {
    int n = 0;
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] == EMPTY) cols[n++] = c;
    }
    return n;
}

int aiChooseMoveEasy(const char board[ROWS][COLS]) {
    /*
        רמה קלה:
        פשוט לבחור עמודה חוקית באקראי.
        זה AI "טיפש" אבל חוקי.
    */
    int cols[COLS];
    int n = validColumns(board, cols);
    if (n == 0) return -1;
    return cols[rand() % n];
}

int aiChooseMoveMedium(const char board[ROWS][COLS], char aiDisc, char humanDisc) {
    /*
        רמה בינונית (היוריסטיקה בסיסית):
        1) אם אני יכול לנצח במהלך אחד -> לשחק את המהלך הזה
        2) אם היריב יכול לנצח במהלך אחד -> לחסום אותו
        3) אחרת -> העדפת מרכז (כי במרכז יש יותר אופציות) ואם לא אפשרי אז אקראי
    */
    char tmp[ROWS][COLS];

    // 1) בדיקת מהלך מנצח למחשב
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] != EMPTY) continue;
        memcpy(tmp, board, sizeof(tmp));
        dropDisc(tmp, c, aiDisc);
        if (checkWin(tmp, aiDisc)) return c;
    }

    // 2) בדיקת חסימה (מהלך מנצח לשחקן)
    for (int c = 0; c < COLS; c++) {
        if (board[0][c] != EMPTY) continue;
        memcpy(tmp, board, sizeof(tmp));
        dropDisc(tmp, c, humanDisc);
        if (checkWin(tmp, humanDisc)) return c;
    }

    // 3) העדפת מרכז
    int center = COLS / 2; // 3
    if (board[0][center] == EMPTY) return center;

    return aiChooseMoveEasy(board);
}

/* ---------- רמה קשה: Minimax + Alpha-Beta ---------- */

/*
    ניקוד לחלון של 4 תאים:
    הרעיון: לתת ניקוד למצבים "קרובים לניצחון" ולתת ניקוד שלילי למצבים
    שבהם היריב קרוב לניצחון (כלומר צריך לחסום).
*/
static int evaluateWindow(const char w[4], char ai, char human) {
    int aiCount = 0, humanCount = 0, emptyCount = 0;
    for (int i = 0; i < 4; i++) {
        if (w[i] == ai) aiCount++;
        else if (w[i] == human) humanCount++;
        else emptyCount++;
    }

    // מצבי קצה חזקים
    if (aiCount == 4) return 100000;
    if (humanCount == 4) return -100000;

    // מצבים "שווים" - לא ניקוד
    if (aiCount > 0 && humanCount > 0) return 0;

    int score = 0;

    // קידום התקפה
    if (aiCount == 3 && emptyCount == 1) score += 100;
    if (aiCount == 2 && emptyCount == 2) score += 10;

    // חסימה - להעניש מצב שבו היריב מתקרב ל-4
    if (humanCount == 3 && emptyCount == 1) score -= 120;
    if (humanCount == 2 && emptyCount == 2) score -= 10;

    return score;
}

/*
    נותן ניקוד לכל הלוח:
    - העדפת מרכז (יותר קומבינציות)
    - סריקת כל חלונות ה-4 האפשריים אופקי/אנכי/אלכסונים
*/
static int scorePosition(const char board[ROWS][COLS], char ai, char human) {
    int score = 0;

    // העדפת מרכז
    int center = COLS / 2;
    int centerCount = 0;
    for (int r = 0; r < ROWS; r++) {
        if (board[r][center] == ai) centerCount++;
    }
    score += centerCount * 6;

    // אופקי
    for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            char w[4] = { board[r][c], board[r][c+1], board[r][c+2], board[r][c+3] };
            score += evaluateWindow(w, ai, human);
        }
    }

    // אנכי
    for (int c = 0; c < COLS; c++) {
        for (int r = 0; r <= ROWS - 4; r++) {
            char w[4] = { board[r][c], board[r+1][c], board[r+2][c], board[r+3][c] };
            score += evaluateWindow(w, ai, human);
        }
    }

    // אלכסון יורד-ימין
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 0; c <= COLS - 4; c++) {
            char w[4] = { board[r][c], board[r+1][c+1], board[r+2][c+2], board[r+3][c+3] };
            score += evaluateWindow(w, ai, human);
        }
    }

    // אלכסון יורד-שמאל
    for (int r = 0; r <= ROWS - 4; r++) {
        for (int c = 3; c < COLS; c++) {
            char w[4] = { board[r][c], board[r+1][c-1], board[r+2][c-2], board[r+3][c-3] };
            score += evaluateWindow(w, ai, human);
        }
    }

    return score;
}

/*
    Minimax:
    maximizing = 1 => תור המחשב (מנסה למקסם ניקוד)
    maximizing = 0 => תור האדם (מנסה למזער ניקוד)
    alpha/beta => גיזום כדי לא לבדוק את כל העץ (יעילות)
*/
static int minimax(const char board[ROWS][COLS], int depth, int maximizing,
                   char ai, char human, int alpha, int beta, int *bestCol) {

    // עצירת רקורסיה: ניצחון/הפסד/תיקו/עומק 0
    if (checkWin(board, ai)) return 1000000;
    if (checkWin(board, human)) return -1000000;
    if (isBoardFull(board) || depth == 0) return scorePosition(board, ai, human);

    int cols[COLS];
    int n = validColumns(board, cols);
    if (n == 0) return 0;

    if (maximizing) {
        int bestScore = INT_MIN;
        int localBest = cols[0];

        for (int i = 0; i < n; i++) {
            int c = cols[i];

            // יוצרים לוח זמני כדי "לנסות" מהלך בלי לשנות את הלוח האמיתי
            char tmp[ROWS][COLS];
            memcpy(tmp, board, sizeof(tmp));
            dropDisc(tmp, c, ai);

            int score = minimax(tmp, depth - 1, 0, ai, human, alpha, beta, NULL);

            if (score > bestScore) {
                bestScore = score;
                localBest = c;
            }

            // עדכון alpha וגיזום
            if (bestScore > alpha) alpha = bestScore;
            if (alpha >= beta) break;
        }

        if (bestCol) *bestCol = localBest;
        return bestScore;
    } else {
        int bestScore = INT_MAX;

        for (int i = 0; i < n; i++) {
            int c = cols[i];

            char tmp[ROWS][COLS];
            memcpy(tmp, board, sizeof(tmp));
            dropDisc(tmp, c, human);

            int score = minimax(tmp, depth - 1, 1, ai, human, alpha, beta, NULL);

            if (score < bestScore) bestScore = score;

            // עדכון beta וגיזום
            if (bestScore < beta) beta = bestScore;
            if (alpha >= beta) break;
        }
        return bestScore;
    }
}

int aiChooseMoveHard(const char board[ROWS][COLS], char aiDisc, char humanDisc) {
    /*
        רמה קשה:
        מפעילים Minimax בעומק קטן (למשל 4) כדי לשמור על ריצה מהירה.
        זה מספיק כדי שה-AI "יראה קדימה" כמה מהלכים ויהיה חזק משמעותית.
    */
    int bestCol = -1;
    minimax(board, 4, 1, aiDisc, humanDisc, INT_MIN, INT_MAX, &bestCol);

    if (bestCol == -1) return aiChooseMoveEasy(board);
    return bestCol;
}
