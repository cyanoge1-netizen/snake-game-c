#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <termios.h>
#include <unistd.h>

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

int main() {
    int height = 10, width = 20, i, j;
    int x = 10, y = 5;
    int score = 0;
    int isPaused = 0;
    enum Direction dir = DIR_STOP;

    // Store tail segment coordinates
    int tailX[100] = {0}, tailY[100] = {0};
    int tailLength = 0;

    // Seed random generator for food
    srand(time(NULL));

    // Place initial food inside the boundary
    int foodX = (rand() % (width - 2)) + 1;
    int foodY = (rand() % (height - 2)) + 1;

    // Setup terminal
    enableRawMode();
    system("clear");

    while (1) {
        // Move cursor back to top-left to redraw without screen flicker
        printf("\033[H");

        // Draw game board
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    printf("#"); // Wall
                }
                else if (i == y && j == x) {
                    printf("O"); // Snake head
                }
                else if (i == foodY && j == foodX) {
                    printf("*"); // Food
                }
                else {
                    // Check if this cell has a body part
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

        printf("Score: %-4d\n", score);
        if (isPaused) {
            printf("Game paused! Press P to resume, Q to exit            \n");
        } else if (dir == DIR_STOP) {
            printf("Controls: Press W/A/S/D or Arrows to start (Q to exit) \n");
        } else {
            printf("Controls: W/A/S/D or Arrows, P to pause, Q to exit     \n");
        }
        fflush(stdout);

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

            // Wall collision
            if (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1) {
                system("clear");
                printf("\n💥 Game Over! You hit the wall!\n");
                printf("Final Score: %d\n\n", score);
                break;
            }

            // Self-collision: did head hit any tail piece?
            int selfCollision = 0;
            for (int k = 0; k < tailLength; k++) {
                if (tailX[k] == x && tailY[k] == y) {
                    selfCollision = 1;
                    break;
                }
            }

            if (selfCollision) {
                system("clear");
                printf("\n💥 Game Over! You bit your own tail!\n");
                printf("Final Score: %d\n\n", score);
                break;
            }

            // Ate food: add point, grow tail, spawn new food
            if (x == foodX && y == foodY) {
                score++;
                if (tailLength < 99) {
                    tailLength++; // Grow by 1 segment
                }

                // Pick a new food spot not on the snake
                int onSnake;
                do {
                    onSnake = 0;
                    foodX = (rand() % (width - 2)) + 1;
                    foodY = (rand() % (height - 2)) + 1;

                    if (foodX == x && foodY == y) {
                        onSnake = 1;
                    } else {
                        for (int k = 0; k < tailLength; k++) {
                            if (tailX[k] == foodX && tailY[k] == foodY) {
                                onSnake = 1;
                                break;
                            }
                        }
                    }
                } while (onSnake);
            }
        }

        // Frame delay (~120ms tick)
        usleep(120000);
    }

    return 0;
}
