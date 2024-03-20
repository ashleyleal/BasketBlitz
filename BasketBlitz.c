/*
 * ECE243 Final Project: BasketBlitz
 * File:   BasketBlitz.c
 * Created on: Mar 18 2024
 * Authors: Ashley Leal and Zuha Mujib
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Constants */
#define GRAVITY 9.81

/* VGA Display Parameters */
volatile int pixel_buffer_start; 
short int Buffer1[240][512];
short int Buffer2[240][512];
volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

/* Structs */
typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int x;
    int y;
} Velocity;

typedef struct {
    int score;
    char name[10]; // 10 characters max name
} Player;

typedef struct {
    Position initialPos;
    Position currentPos;
    float startingAngle;
    Velocity initialVel;
    Velocity currentVel;
    bool isMoving;
    Player owner;
} Basketball;

typedef struct {
    Player currentTurn;
    Basketball currentBall;
    Player player1;
    Player player2;
    int currentTime;
    int maxTime;
    int currentRound;
    int maxRounds;
    bool isGameOver;
} Game;

/* VGA Graphics Functions Prototypes */
void plot_pixel(int x, int y, short int pixel_color);
void clear_screen();
void wait_for_vsync();

/* Game Logic Functions Prototypes */
void initializeGame(Game *game);
void updateBasketball(Basketball *ball, int deltaTime);
void updateGame(Game *game, int deltaTime);


int main(void) {

    /* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int)&Buffer1;  // first store the address in the  back buffer
    /* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync();
    /* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen();  // pixel_buffer_start points to the pixel buffer

    /* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int)&Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1);  // we draw on the back buffer
    clear_screen();                              // pixel_buffer_start points to the pixel buffer

    // Initialize the game
    Game game;
    initializeGame(&game);

    // Main game loop
    while (1) {

        wait_for_vsync(); // swap front and back buffers on VGA vertical sync
        pixel_buffer_start = *(pixel_ctrl_ptr + 1); // point to the back buffer
    }

    // Initialize the VGA display
    return 0;
}

/* -------------------------------------------------------------------------- */
/*                       VGA Graphics subroutines                             */
/* -------------------------------------------------------------------------- */

void plot_pixel(int x, int y, short int pixel_color) {
    volatile short int *one_pixel_address;
    one_pixel_address = pixel_buffer_start + (y << 10) + (x << 1);
    *one_pixel_address = pixel_color;
}

void clear_screen() {
    int x, y;
    for (x = 0; x < 320; x++)
        for (y = 0; y < 240; y++)
            plot_pixel(x, y, 0x0000);  // clear pixel at (x, y)
}

void wait_for_vsync() {
    {
        int status;
        *pixel_ctrl_ptr = 1;  // start the synchronization process
        // - write 1 into front buffer address register
        status = *(pixel_ctrl_ptr + 3);  // read the status register
        while ((status & 0x01) != 0)     // polling loop waiting for S bit to go to 0
        {
            status = *(pixel_ctrl_ptr + 3);
        }
    }
}

/* -------------------------------------------------------------------------- */
/*                         Game Logic subroutines                             */
/* -------------------------------------------------------------------------- */

void initializeGame(Game *game) {
    // Initialize the players
    game->player1.score = 0;
    game->player2.score = 0;

    // Set the names of the players
    strcpy(game->player1.name, "Player 1"); // make this take user input eventually but hardcode for now
    strcpy(game->player2.name, "Player 2");
    
    // Initialize the basketball
    game->currentBall.initialPos.x = 0;
    game->currentBall.initialPos.y = 0;
    game->currentBall.currentPos.x = 0;
    game->currentBall.currentPos.y = 0;
    game->currentBall.startingAngle = 0;
    game->currentBall.initialVel.x = 0;
    game->currentBall.initialVel.y = 0;
    game->currentBall.currentVel.x = 0;
    game->currentBall.currentVel.y = 0;
    game->currentBall.isMoving = false;
    game->currentBall.owner = game->player1;
    
    // Initialize the game
    game->currentTurn = game->player1;
    game->currentTime = 0;
    game->maxTime = 60;
    game->currentRound = 1;
    game->maxRounds = 3;
    game->isGameOver = false;
}


void updateBasketball(Basketball *ball, int deltaTime) {
    
    // If the basketball is not moving, do not update its position or velocity
    if (!ball->isMoving) {
        return;
    }
    
    // Update the position of the basketball
    ball->currentPos.x += ball->currentVel.x * deltaTime; // x = v0t
    ball->currentPos.y += ball->currentVel.y * deltaTime - (0.5 * GRAVITY) * deltaTime * deltaTime; // y = v0t - 0.5gt^2

    // Update the velocity of the basketball
    // x velocity remains constant
    ball->currentVel.y -= GRAVITY * deltaTime; // v = v0 - gt

    // Check if the basketball has hit the ground
    if (ball->currentPos.y <= 0) {
        ball->currentPos.y = 0;
        ball->currentVel.y = 0;
        ball->isMoving = false;
    }
}

void updateGame(Game *game, int deltaTime) {
    // Update the basketball
    updateBasketball(&game->currentBall, deltaTime);
    
    // Update the time
    game->currentTime += deltaTime;
    
    // Check if the game is over
    if (game->currentTime >= game->maxTime) {
        game->isGameOver = true;
    }
}


