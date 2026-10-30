#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Movement directions
enum Direction {
    DIR_STOP = 0,
    DIR_UP,
    DIR_DOWN,
    DIR_LEFT,
    DIR_RIGHT
};

int main() {
    int height = 10, width = 20, i, j;
    int x = 10, y = 5;
    int score = 0;
    char move;
    enum Direction dir = DIR_STOP;

    // Store tail segment coordinates
    int tailX[100] = {0}, tailY[100] = {0};
    int tailLength = 0;

    // Seed random generator for food
    srand(time(NULL));

    // Place initial food inside the boundary
    int foodX = (rand() % (width - 2)) + 1;
    int foodY = (rand() % (height - 2)) + 1;

    while (1) {
        system("clear");

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

        printf("Score: %d\n", score);
        printf("Move (w/a/s/d or 2/4/6/8, q to exit): ");
        fflush(stdout);

        // Read move
        if (scanf(" %c", &move) != 1) {
            break;
        }

        // Flush extra input safely
        int flushChar;
        while ((flushChar = getchar()) != '\n' && flushChar != EOF);

        // Quit if Q is pressed
        if (move == 'q' || move == 'Q') {
            printf("Exiting Game...\n");
            break;
        }

        // Pick new direction from key
        enum Direction newDir = dir;
        if (move == 'w' || move == 'W' || move == '2')
            newDir = DIR_UP;
        else if (move == 's' || move == 'S' || move == '8')
            newDir = DIR_DOWN;
        else if (move == 'd' || move == 'D' || move == '6')
            newDir = DIR_RIGHT;
        else if (move == 'a' || move == 'A' || move == '4')
            newDir = DIR_LEFT;
        else {
            // Ignore unrecognized keys without moving
            continue;
        }

        // Don't let snake turn directly backwards into itself
        if (tailLength > 0) {
            if ((dir == DIR_UP && newDir == DIR_DOWN) ||
                (dir == DIR_DOWN && newDir == DIR_UP) ||
                (dir == DIR_LEFT && newDir == DIR_RIGHT) ||
                (dir == DIR_RIGHT && newDir == DIR_LEFT)) {
                continue; // Ignore opposite direction
            }
        }
        dir = newDir;

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

    return 0;
}
