#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "ai.h"


/*
    מבנה סטטיסטיקות פשוט.
    נשמר בזיכרון במהלך הריצה (לא קובץ) – מספיק לפי דרישות בסיסיות של מטלות מבוא.
*/
typedef struct {
    int games;
    int pvp_p1_wins;
    int pvp_p2_wins;
    int pvc_human_wins;
    int pvc_cpu_wins;
    int ties;
} Stats;

/*
    ניקוי שאריות מה-buffer:
    חשוב כי scanf עלול להשאיר '\n' או טקסט לא נקרא שידפוק קלטים הבאים.
*/
static void clearLine(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

/*
    קריאת מספר שלם בטווח [min, max] עם ולידציה:
    - אם המשתמש מכניס טקסט/תו => scanf נכשל => ננקה ונבקש שוב
    - אם המספר מחוץ לטווח => נבקש שוב
*/
static int readIntInRange(const char* prompt, int min, int max) {
    int x;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &x) != 1) {
            clearLine();
            printf("Invalid input. Try again.\n");
            continue;
        }
        clearLine();
        if (x < min || x > max) {
            printf("Please choose a number between %d and %d.\n", min, max);
            continue;
        }
        return x;
    }
}

/*
    הדפסת סטטיסטיקות נוכחיות:
    הצגה קצרה של מספר משחקים וניצחונות לפי מצבי המשחק.
*/
static void printStats(const Stats* s) {
    printf("\n--- Statistics ---\n");
    printf("Total games: %d\n", s->games);
    printf("PvP - Player 1 wins: %d\n", s->pvp_p1_wins);
    printf("PvP - Player 2 wins: %d\n", s->pvp_p2_wins);
    printf("PvC - Human wins: %d\n", s->pvc_human_wins);
    printf("PvC - Computer wins: %d\n", s->pvc_cpu_wins);
    printf("Ties: %d\n", s->ties);
}

/*
    משחק שחקן מול שחקן:
    - X מתחיל
    - כל תור: מבקשים עמודה 1..7, מפילים דיסק
    - אחרי כל מהלך: בודקים ניצחון, אחר כך תיקו
*/
static void playPvP(Stats* stats) {
    char board[ROWS][COLS];
    initBoard(board);

    const char P1 = 'X';
    const char P2 = 'O';
    char current = P1;

    while (1) {
        printBoard(board);

        int col = readIntInRange(
            (current == P1) ? "Player 1 (X) choose column 1-7: " : "Player 2 (O) choose column 1-7: ",
            1, 7
        ) - 1; // ממירים ל-0..6

        if (dropDisc(board, col, current) == -1) {
            // עמודה מלאה או לא חוקית
            printf("Column is full/invalid. Try again.\n");
            continue;
        }

        if (checkWin(board, current)) {
            printBoard(board);
            printf("%c wins!\n", current);

            stats->games++;
            if (current == P1) stats->pvp_p1_wins++;
            else stats->pvp_p2_wins++;
            return;
        }

        if (isBoardFull(board)) {
            printBoard(board);
            printf("It's a tie!\n");

            stats->games++;
            stats->ties++;
            return;
        }

        // מעבר תור
        current = (current == P1) ? P2 : P1;
    }
}

/*
    משחק משתמש מול מחשב:
    - המשתמש X, המחשב O
    - 3 רמות: קל/בינוני/קשה
*/
static void playPvC(Stats* stats) {
    char board[ROWS][COLS];
    initBoard(board);

    int level = readIntInRange("Choose level: 1-Easy, 2-Medium, 3-Hard: ", 1, 3);

    const char HUMAN = 'X';
    const char CPU   = 'O';
    char current = HUMAN;

    while (1) {
        printBoard(board);

        if (current == HUMAN) {
            int col = readIntInRange("You (X) choose column 1-7: ", 1, 7) - 1;

            if (dropDisc(board, col, HUMAN) == -1) {
                printf("Column is full/invalid. Try again.\n");
                continue;
            }
        } else {
            int col;
            if (level == 1) col = aiChooseMoveEasy(board);
            else if (level == 2) col = aiChooseMoveMedium(board, CPU, HUMAN);
            else col = aiChooseMoveHard(board, CPU, HUMAN);

            printf("Computer (O) chose column %d\n", col + 1);
            dropDisc(board, col, CPU);
        }

        if (checkWin(board, current)) {
            printBoard(board);

            if (current == HUMAN) {
                printf("You win!\n");
                stats->games++;
                stats->pvc_human_wins++;
            } else {
                printf("Computer wins!\n");
                stats->games++;
                stats->pvc_cpu_wins++;
            }
            return;
        }

        if (isBoardFull(board)) {
            printBoard(board);
            printf("It's a tie!\n");

            stats->games++;
            stats->ties++;
            return;
        }

        current = (current == HUMAN) ? CPU : HUMAN;
    }
}

/*
    תפריט ראשי:
    1) משתמש מול משתמש
    2) משתמש מול מחשב
    3) סטטיסטיקות
    0) יציאה
*/
static int menu(void) {
    printf("\n=== CONNECT 4 ===\n");
    printf("1) Player vs Player\n");
    printf("2) Player vs Computer\n");
    printf("3) Show Statistics\n");
    printf("0) Exit\n");
    return readIntInRange("Choose option: ", 0, 3);
}

int main(void) {
    // אתחול אקראיות פעם אחת בלבד בתחילת התוכנית
    srand((unsigned)time(NULL));

    Stats stats = {0};

    while (1) {
        int choice = menu();
        if (choice == 0) break;

        if (choice == 1) playPvP(&stats);
        else if (choice == 2) playPvC(&stats);
        else if (choice == 3) printStats(&stats);
    }

    printf("Goodbye!\n");
    return 0;
}
