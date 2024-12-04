#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <unistd.h> // For usleep on Linux or macOS

#define WIDTH 20
#define HEIGHT 20
#define MAX_SCORE 10

typedef struct SnakeSegment {
    int x;
    int y;
    struct SnakeSegment* next;
} SnakeSegment;

SnakeSegment* snakeHead = NULL; // Head of the snake linked list
int foodX, foodY;
int score;
int gameOver;
char direction; // Current direction
int speed;      // Speed of the snake (delay in microseconds)

// Function prototypes
void initializeGame();
void placeFood();
void renderBoard();
void getInput();
void updateSnake();
int checkCollision();
void chooseDifficulty();
void addSegment();
void moveSnake();

int main() {
    char choice;

    chooseDifficulty(); // Choose difficulty level

    do {
        initializeGame(); // Initialize game variables

        while (!gameOver) {
            renderBoard();
            getInput();
            updateSnake();

            // Check for collisions or if max score is reached
            if (checkCollision()) {
                gameOver = 1; // Set game over if a collision is detected
            }
            if (score == MAX_SCORE) {
                break; // Exit the loop if max score is reached
            }
            usleep(speed); // Sleep for specified speed to control movement speed
        }

        system("cls"); // Clear console screen before showing game over message
        if (score == MAX_SCORE) {
            printf("Congratulations! You've reached the maximum score!\n");
            break; // Exit the loop if max score is reached
        } else {
            printf("Game Over! Your score: %d\n", score);
        }

        // Ask player if they want to retry or exit
        printf("Is this all You Got? (y/n): ");
        choice = _getch(); // Get input without waiting for enter

    } while (choice == 'N' || choice == 'n');

    return 0;
}

void chooseDifficulty() {
    char difficulty;
    printf("Choose Difficulty Level:\n");
    printf("1. Easy\n");
    printf("2. Medium\n");
    printf("3. Hard\n");
    printf("Enter your choice (1-3): ");
    difficulty = _getch(); // Read character input

    switch (difficulty) {
        case '1':
            speed = 100000; // Slow speed for easy
            break;
        case '2':
            speed = 75000; // Normal speed for medium
            break;
        case '3':
            speed = 50000;  // Fast speed for hard
            break;
        default:
            printf("Invalid choice. Defaulting to Medium.\n");
            speed = 75000; // Default to medium speed
            break;
    }
}

void initializeGame() {
    score = 0;       // Reset score
    gameOver = 0;    // Reset game over flag
    direction = 'd'; // Reset direction to right

    // Initialize snake with one segment at the center of the board
    snakeHead = (SnakeSegment*)malloc(sizeof(SnakeSegment));
    snakeHead->x = WIDTH / 2;
    snakeHead->y = HEIGHT / 2;
    snakeHead->next = NULL;

    placeFood();
}

void placeFood() {
    int validPosition = 0;
    while (!validPosition) {
        foodX = rand() % (WIDTH - 2) + 1; // Ensure food is within bounds
        foodY = rand() % (HEIGHT - 2) + 1; // Ensure food is within bounds

        validPosition = 1; // Assume it's valid until proven otherwise

        SnakeSegment* current = snakeHead;
        while (current != NULL) {
            if (current->x == foodX && current->y == foodY) {
                validPosition = 0; // Found a collision with the snake
                break; // Exit loop
            }
            current = current->next;
        }
    }
}

void renderBoard() {
    system("cls"); // Clear console screen (Windows)

    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == foodY && j == foodX) {
                printf("*"); // Food representation
            } else if (i == snakeHead->y && j == snakeHead->x) {
                printf("O"); // Snake head representation
            } else if (i == 0 || i == HEIGHT - 1 || j == 0 || j == WIDTH - 1) {
                printf("x"); // Boundary representation
            } else {
                int isBodyPart = 0;
                int segmentCount = 0;  // Counter to alternate body characters

                SnakeSegment* current = snakeHead->next;
                while (current != NULL) {
                    if (i == current->y && j == current->x) {
                        // Alternate between "/" and "\"
                        if (segmentCount % 2 == 0) {
                            printf("/"); // Even segments
                        } else {
                            printf("\\"); // Odd segments
                        }
                        isBodyPart = 1;
                        break;
                    }
                    current = current->next;
                    segmentCount++;
                }

                if (!isBodyPart) printf(" "); // Empty space
            }
        }
        printf("\n");
    }

    printf("Score: %d\n", score);
}

void getInput() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'w':
                if (direction != 's') direction = 'w';
                break;
            case 's':
                if (direction != 'w') direction = 's';
                break;
            case 'a':
                if (direction != 'd') direction = 'a';
                break;
            case 'd':
                if (direction != 'a') direction = 'd';
                break;
        }
    }
}

void addSegment() {
    // Add a new segment to the tail of the snake
    SnakeSegment* newSegment = (SnakeSegment*)malloc(sizeof(SnakeSegment));

    SnakeSegment* temp = snakeHead;
    while (temp->next != NULL) {
        temp = temp->next;
    }

    // Set the new segment's position to match the last tail's position
    newSegment->x = temp->x;
    newSegment->y = temp->y;
    newSegment->next = NULL;

    temp->next = newSegment; // Attach the new segment to the end of the snake
}

void updateSnake() {
    int prevX = snakeHead->x;
    int prevY = snakeHead->y;
    int tempX, tempY;

    moveSnake(); // Move the snake's head based on input

    // If the snake eats the food
    if (snakeHead->x == foodX && snakeHead->y == foodY) {
        score++;
        placeFood();   // Place a new food
        addSegment();  // Add a new segment to the snake
    }

    // Move each segment to the position of the one in front of it
    SnakeSegment* current = snakeHead->next;
    while (current != NULL) {
        tempX = current->x;
        tempY = current->y;
        current->x = prevX;
        current->y = prevY;
        prevX = tempX;
        prevY = tempY;
        current = current->next;
    }
}

void moveSnake() {
    // Move the head in the current direction
    switch (direction) {
        case 'w':
            snakeHead->y--;
            break;
        case 's':
            snakeHead->y++;
            break;
        case 'a':
            snakeHead->x--;
            break;
        case 'd':
            snakeHead->x++;
            break;
    }
}

int checkCollision() {
    // Check if snake collides with the walls
    if (snakeHead->x <= 0 || snakeHead->x >= WIDTH - 1 ||
        snakeHead->y <= 0 || snakeHead->y >= HEIGHT - 1) {
        return 1;
    }

    // Check if snake collides with itself
    SnakeSegment* current = snakeHead->next;
    while (current != NULL) {
        if (current->x == snakeHead->x && current->y == snakeHead->y) {
            return 1;
        }
        current = current->next;
    }

    return 0;
}
