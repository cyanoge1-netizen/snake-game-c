#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main() {
    int height = 10, width = 20, i, j;
    int x = 10, y = 5;
    int score = 0;
    char move;

    // Body segment tracking arrays and length counter
    int tailX[100] = {0}, tailY[100] = {0};
    int tailLength = 0;

    // Seed pseudo-random generator
    srand(time(NULL));

    // Initial food coordinates (bounded within walls)
    int foodX = (rand() % (width - 2)) + 1;
    int foodY = (rand() % (height - 2)) + 1;

    while (1) {
        system("clear");

        // Render game board
        for (i = 0; i < height; i++) {
            for (j = 0; j < width; j++) {
                if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                    printf("#"); // Boundary wall
                }
                else if (i == y && j == x) {
                    printf("O"); // Snake head
                }
                else if (i == foodY && j == foodX) {
                    printf("*"); // Food token
                }
                else {
                    // Check if current coordinate belongs to a body segment
                    int printTail = 0;
                    for (int k = 0; k < tailLength; k++) {
                        if (tailX[k] == j && tailY[k] == i) {
                            printf("o"); // Body segment
                            printTail = 1;
                            break;
                        }
                    }
                    if (!printTail) {
                        printf(" "); // Empty playable space
                    }
                }
            }
            printf("\n");
        }

        printf("Score: %d\n", score);
        printf("Move (w/a/s/d or 2/4/6/8, q to exit): ");
        fflush(stdout);

        // Capture input and handle unexpected EOF
        if (scanf(" %c", &move) != 1) {
            break;
        }

        // Flush standard input buffer to discard trailing newline
        int flushChar;
        while ((flushChar = getchar()) != '\n' && flushChar != EOF);

        // Check for termination command
        if (move == 'q' || move == 'Q') {
            printf("Exiting Game...\n");
            break;
        }

        // Shift register logic: propagate coordinates down the body segments
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

        // Update head coordinates based on input
        if (move == 'w' || move == 'W' || move == '2')
            y--; // Move up
        else if (move == 's' || move == 'S' || move == '8')
            y++; // Move down
        else if (move == 'd' || move == 'D' || move == '6')
            x++; // Move right
        else if (move == 'a' || move == 'A' || move == '4')
            x--; // Move left

        // Boundary collision detection
        if (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1) {
            system("clear");
            printf("\n💥 Game Over! You've Hit the Wall!!\n");
            printf("Final Score: %d\n\n", score);
            break;
        }

        // Self-collision detection: check if head collides with any body segment
        int selfCollision = 0;
        for (int k = 0; k < tailLength; k++) {
            if (tailX[k] == x && tailY[k] == y) {
                selfCollision = 1;
                break;
            }
        }

        if (selfCollision) {
            system("clear");
            printf("\n💥 Game Over! You bit your own tail!!\n");
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
