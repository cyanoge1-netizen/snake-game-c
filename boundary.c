#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

#define BOARD_WIDTH 20
#define BOARD_HEIGHT 10
#define MAX_TAIL 100

// Movement directions
enum Direction {
    DIR_STOP = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

static struct termios orig_termios;

// Restore terminal settings and show cursor when exiting
void disableRawMode(void) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    printf("\033[?25h"); // Show cursor
    fflush(stdout);
}

// Turn on raw mode so keys are read immediately without waiting for Enter
void enableRawMode(void) {
    if (!isatty(STDIN_FILENO)) {
        return;
    }
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(disableRawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON); // Don't print typed keys, don't wait for Enter
    raw.c_cc[VMIN] = 0;              // Return right away even if no key was pressed
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    printf("\033[?25l"); // Hide cursor while playing
    fflush(stdout);
}

// Read non-blocking key press, including arrow keys
char readKey(void) {
    char c = 0;
    if (read(STDIN_FILENO, &c, 1) == 1) {
        if (c == '\033') {
            char seq[2];
            if (read(STDIN_FILENO, &seq[0], 1) == 1 && read(STDIN_FILENO, &seq[1], 1) == 1) {
                if (seq[0] == '[') {
                    switch (seq[1]) {
                        case 'A': return 'w'; // Up arrow
                        case 'B': return 's'; // Down arrow
                        case 'C': return 'd'; // Right arrow
                        case 'D': return 'a'; // Left arrow
                    }
                }
            }
            return 0; // Other escape sequence
        }
        return c;
    }
    return 0;
}

// Load high score from file, or return 0 if file doesn't exist
int loadHighScore(void) {
    int highScore = 0;
    FILE *f = fopen("highscore.txt", "r");
    if (f != NULL) {
        if (fscanf(f, "%d", &highScore) != 1) {
            highScore = 0;
        }
        fclose(f);
    }
    return highScore;
}

// Save high score to file
void saveHighScore(int highScore) {
    FILE *f = fopen("highscore.txt", "w");
    if (f != NULL) {
        fprintf(f, "%d\n", highScore);
        fclose(f);
    }
}

// Prompt player to choose game speed before starting
int chooseDifficulty(void) {
    char choice;
    system("clear");
    printf("========================\n");
    printf("     Snake Game in C    \n");
    printf("========================\n\n");
    printf("Choose difficulty:\n");
    printf("  1. Easy   (Slow)\n");
    printf("  2. Medium (Normal)\n");
    printf("  3. Hard   (Fast)\n\n");
    printf("Select (1-3, default 2): ");
    fflush(stdout);

    if (scanf(" %c", &choice) == 1) {
        if (choice == '1') return 170000; // 170ms
        if (choice == '3') return 70000;  // 70ms
    }
    return 110000; // default 110ms (Medium)
}

// Spawn food on a random empty tile
void spawnFood(int *foodX, int *foodY, int headX, int headY, const int tailX[], const int tailY[], int tailLength) {
    int onSnake;
    do {
        onSnake = 0;
        *foodX = (rand() % (BOARD_WIDTH - 2)) + 1;
        *foodY = (rand() % (BOARD_HEIGHT - 2)) + 1;

        if (*foodX == headX && *foodY == headY) {
            onSnake = 1;
        } else {
            for (int k = 0; k < tailLength; k++) {
                if (tailX[k] == *foodX && tailY[k] == *foodY) {
                    onSnake = 1;
                    break;
                }
            }
        }
    } while (onSnake);
}

// Draw the walls, snake, food, and score banner
void drawBoard(int headX, int headY, int foodX, int foodY, const int tailX[], const int tailY[], int tailLength, int score, int highScore, int isPaused, enum Direction dir) {
    printf("\033[H"); // Move cursor back to top-left to redraw without screen flicker

    for (int i = 0; i < BOARD_HEIGHT; i++) {
        for (int j = 0; j < BOARD_WIDTH; j++) {
            if (i == 0 || i == BOARD_HEIGHT - 1 || j == 0 || j == BOARD_WIDTH - 1) {
                printf("#"); // Wall
            } else if (i == headY && j == headX) {
                printf("O"); // Snake head
            } else if (i == foodY && j == foodX) {
                printf("*"); // Food
            } else {
                int printTail = 0;
                for (int k = 0; k < tailLength; k++) {
                    if (tailX[k] == j && tailY[k] == i) {
                        printf("o"); // Body part
                        printTail = 1;
                        break;
                    }
                }
                if (!printTail) {
                    printf(" "); // Empty space
                }
            }
        }
        printf("\n");
    }

    printf("Score: %-4d | High Score: %-4d\n", score, highScore);
    if (isPaused) {
        printf("Game paused! Press P to resume, Q to exit            \n");
    } else if (dir == DIR_STOP) {
        printf("Controls: Press W/A/S/D or Arrows to start (Q to exit) \n");
    } else {
        printf("Controls: W/A/S/D or Arrows, P to pause, Q to exit     \n");
    }
    fflush(stdout);
}

int main(void) {
    int x = 10, y = 5;
    int score = 0;
    int highScore = loadHighScore();
    int isPaused = 0;
    int speedDelay = chooseDifficulty();
    enum Direction dir = DIR_STOP;

    // Store tail segment coordinates
    int tailX[MAX_TAIL] = {0}, tailY[MAX_TAIL] = {0};
    int tailLength = 0;

    // Seed random generator for food
    srand((unsigned int)time(NULL));

    // Place initial food inside boundary
    int foodX, foodY;
    spawnFood(&foodX, &foodY, x, y, tailX, tailY, tailLength);

    // Setup terminal
    enableRawMode();
    system("clear");

    while (1) {
        // Render current game state
        drawBoard(x, y, foodX, foodY, tailX, tailY, tailLength, score, highScore, isPaused, dir);

        // Check for player input
        char key = readKey();

        // Quit game
        if (key == 'q' || key == 'Q') {
            printf("\nExiting Game...\n");
            break;
        }

        // Toggle pause
        if (key == 'p' || key == 'P') {
            isPaused = !isPaused;
        }

        // Steer snake only if not paused
        if (!isPaused) {
            enum Direction newDir = dir;
            if (key == 'w' || key == 'W' || key == '2')
                newDir = DIR_UP;
            else if (key == 's' || key == 'S' || key == '8')
                newDir = DIR_DOWN;
            else if (key == 'd' || key == 'D' || key == '6')
                newDir = DIR_RIGHT;
            else if (key == 'a' || key == 'A' || key == '4')
                newDir = DIR_LEFT;

            // Don't let snake turn directly backwards into itself
            if (newDir != dir && tailLength > 0) {
                if ((dir == DIR_UP && newDir == DIR_DOWN) ||
                    (dir == DIR_DOWN && newDir == DIR_UP) ||
                    (dir == DIR_LEFT && newDir == DIR_RIGHT) ||
                    (dir == DIR_RIGHT && newDir == DIR_LEFT)) {
                    newDir = dir; // Ignore opposite direction
                }
            }
            dir = newDir;
        }

        // Move the snake once started (and not paused)
        if (!isPaused && dir != DIR_STOP) {
            // Move tail: each part takes the position of the one ahead of it
            int prevX = x;
            int prevY = y;
            int prev2X, prev2Y;

            for (int k = 0; k < tailLength; k++) {
                prev2X = tailX[k];
                prev2Y = tailY[k];
                tailX[k] = prevX;
                tailY[k] = prevY;
                prevX = prev2X;
                prevY = prev2Y;
            }

            // Move head in chosen direction
            if (dir == DIR_UP)
                y--; // Move up
            else if (dir == DIR_DOWN)
                y++; // Move down
            else if (dir == DIR_RIGHT)
                x++; // Move right
            else if (dir == DIR_LEFT)
                x--; // Move left

            int gameOver = 0;

            // Check wall collision
            if (x <= 0 || x >= BOARD_WIDTH - 1 || y <= 0 || y >= BOARD_HEIGHT - 1) {
                system("clear");
                printf("\n💥 Game Over! You hit the wall!\n");
                gameOver = 1;
            }

            // Check self-collision
            if (!gameOver) {
                for (int k = 0; k < tailLength; k++) {
                    if (tailX[k] == x && tailY[k] == y) {
                        system("clear");
                        printf("\n💥 Game Over! You bit your own tail!\n");
                        gameOver = 1;
                        break;
                    }
                }
            }

            // If player died, show score and offer to play again
            if (gameOver) {
                printf("Final Score: %d | High Score: %d\n\n", score, highScore);
                printf("Play again? (y/n): ");
                fflush(stdout);

                // Wait for player to press Y or N
                char again = 0;
                while (again != 'y' && again != 'Y' && again != 'n' && again != 'N' && again != 'q' && again != 'Q') {
                    again = readKey();
                    usleep(20000);
                }

                if (again == 'y' || again == 'Y') {
                    // Reset snake state for another round
                    x = 10;
                    y = 5;
                    score = 0;
                    tailLength = 0;
                    dir = DIR_STOP;
                    isPaused = 0;
                    spawnFood(&foodX, &foodY, x, y, tailX, tailY, tailLength);
                    system("clear");
                    continue;
                } else {
                    printf("\nThanks for playing!\n");
                    break;
                }
            }

            // Ate food: add point, grow tail, spawn new food
            if (x == foodX && y == foodY) {
                score++;
                if (score > highScore) {
                    highScore = score;
                }
                if (tailLength < MAX_TAIL - 1) {
                    tailLength++; // Grow by 1 segment
                }

                // Pick a new food spot not on the snake
                spawnFood(&foodX, &foodY, x, y, tailX, tailY, tailLength);
            }
        }

        // Frame delay based on chosen difficulty
        usleep(speedDelay);
    }

    // Save high score before exiting
    saveHighScore(highScore);

    return 0;
}
