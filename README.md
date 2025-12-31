
# Four in a Row (C)

Connect Four (“4 בשורה”) implemented in C, including a simple AI opponent.

## Project Files
- `main.c` – Program entry point, game flow
- `game.c` / `game.h` – Game logic (board, moves, win checks)
- `ai.c` / `ai.h` – AI logic (computer player)

## How to Compile & Run (GCC)
### Windows / Linux / macOS (with gcc installed)
From the project folder:

```bash
gcc main.c game.c ai.c -o fourinarow
