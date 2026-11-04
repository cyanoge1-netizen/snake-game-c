# Snake Game in C

A simple terminal Snake game written in pure C without any external graphics libraries. Built for learning C programming, 2D coordinates, terminal handling, and game loops.

## Features

- Real-time movement in the terminal (no need to press Enter after each move)
- Controls with Arrow keys, WASD, or numpad
- Pause and resume anytime with `P`
- Difficulty selection on startup (Easy, Medium, Hard)
- Score tracking and high score saved to `highscore.txt`
- Option to play again right after game over

## Controls

- **Move:** Arrow keys or `W` / `A` / `S` / `D` (also supports `2`, `4`, `6`, `8` on numpad)
- **Pause / Resume:** `P`
- **Quit:** `Q`
- **Play Again:** `Y` (when prompted on game over)

## How to Compile and Run

You just need GCC or Clang on Linux or macOS.

### Compile
```bash
gcc boundary.c -o snake
```

Or with compiler warnings enabled:
```bash
gcc -Wall -Wextra -pedantic boundary.c -o snake
```

### Run
```bash
./snake
```

## How It Works

- The game board is drawn using simple terminal characters (`#` for walls, `O` for head, `o` for tail, `*` for food).
- Terminal raw mode is used so keystrokes are read immediately without waiting for Enter.
- The snake grows whenever it eats food, and the game ends if you hit the wall or your own tail.

## Author

**Suleman Ahmed Shuvo**  
Roll: 43 | Batch: 19  
Department of Computer Science & Engineering  
Sylhet Engineering College  

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
