/*
 * ECE243 Final Project: Basket Blitz
 * File:   BasketBlitz.c
 * Created on: Mar 18 2024
 * Authors: Ashley Leal and Zuha Mujib
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* VGA Display Parameters */
volatile int pixel_buffer_start; 
short int Buffer1[240][512];
short int Buffer2[240][512];
volatile int *pixel_ctrl_ptr = (int *)0xFF203020;

/* VGA Graphics Functions Prototypes */
void plot_pixel(int x, int y, short int pixel_color);
void clear_screen();
void wait_for_vsync();

/* Game Logic Functions Prototypes */

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
    Position initalPos;
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

int main(void) {
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