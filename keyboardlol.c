#include <stdio.h>
#include <math.h>
#include <stdbool.h>

volatile int pixel_buffer_start; // global variable
short int Buffer1[240][512];
short int Buffer2[240][512];

#define GRAVITY 9.81
#define PS2_BASE			0xFF200100

void plot_pixel(int x, int y, short int line_color);
void draw_line(int start_x, int start_y, int end_x, int end_y, short int color);
void swap(int* point1, int* point2);
void clear_screen();
void draw_circle(int x0, int y0, int radius, short int color, bool fill);
void move_ball(unsigned char byte);
void read_keyboard(unsigned char *byte1, unsigned char *byte2, unsigned char *byte3);
void draw_box(int x, int y, short int color);
void drawProjectile(float angle);
void wait_for_vsync();

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int x;
    int y;
} Velocity;

typedef struct {
	int x;
	int y;
	int dx;
	int dy;
} ProjPath;

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

Basketball test_ball;
ProjPath projectile;

unsigned char byte1 = 0, byte2 = 0, byte3 = 0;
volatile int * PS2_ptr = (int *)PS2_BASE;

int main(void)
{
	volatile int * pixel_ctrl_ptr = (int *)0xFF203020;
    /* Read location of the pixel buffer from the pixel buffer controller */
	
	// initialize the original position of the basketball
	test_ball.initialPos.x = 150;
	test_ball.initialPos.y = 150;
	
	test_ball.initialVel.x = 0;
	test_ball.initialVel.y = 0;

	test_ball.currentVel.x = 0;
	test_ball.currentVel.y = 0;
	
	/* set front pixel buffer to Buffer 1 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer1; // first store the address in the  back buffer
	
	/* now, swap the front/back buffers, to set the front buffer location */
    wait_for_vsync(); // poll for s-bit to set to 0
	
	/* initialize a pointer to the pixel buffer, used by drawing functions */
    pixel_buffer_start = *pixel_ctrl_ptr;
    clear_screen(); // pixel_buffer_start points to the pixel buffer
	
	/* set back pixel buffer to Buffer 2 */
    *(pixel_ctrl_ptr + 1) = (int) &Buffer2;
    pixel_buffer_start = *(pixel_ctrl_ptr + 1); // we draw on the back bwe draw on the back yclear_screen();
	
	unsigned char pressedKey = 0;
	*(PS2_ptr) = 0xFF; // reset
	while (1){
		// draw the image on the screen
		draw_circle(test_ball.initialPos.x, test_ball.initialPos.y, 30, 0xFFA200, true);
		
		// update the ball position
		read_keyboard(&byte1, &byte2, &byte3);
		move_ball(byte1);
		
		// swap the front and back buffers
		wait_for_vsync(); // Call the function to synchronize with VGA controller and swap buffers
    	pixel_buffer_start = *(pixel_ctrl_ptr + 1); // Set pixel buffer to the back buffer
		// Clear back buffer
    	clear_screen();
	}
}

void move_ball(unsigned char byte){
	// if pressed key is right arrow key, move right
	if (byte == 0x74) {
		test_ball.initialPos.x += 5;
	} else if (byte == 0x6B) {
		test_ball.initialPos.x -= 5;
	}
}

void read_keyboard(unsigned char *byte1, unsigned char *byte2, unsigned char *byte3) {
    int PS2_data, RVALID;

    // PS/2 mouse needs to be reset (must be already plugged in)
	PS2_data = *(PS2_ptr); // read the Data register in the PS/2 port
	RVALID = PS2_data & 0x8000; // extract the RVALID field
	if (RVALID) {
		/* shift the next data byte into the display */
		*byte1 = *byte2;
		*byte2 = *byte3;
		*byte3 = PS2_data & 0xFF;
		//HEX_PS2(byte1, byte2, byte3);
		if ((*byte2 == (char)0xAA) && (*byte3 == (char)0x00))
			// mouse inserted; initialize sending of data
			*(PS2_ptr) = 0xF4;
	}
}

void clear_screen()
{
        int y, x;

        for (x = 0; x < 320; x++)
                for (y = 0; y < 240; y++)
                        plot_pixel (x, y, 0xfff);
}

// code not shown for clear_screen() and draw_line() subroutines
void plot_pixel(int x, int y, short int line_color)
{
    volatile short int *one_pixel_address;

        one_pixel_address = (short int *)(pixel_buffer_start + (y << 10) + (x << 1));

        *one_pixel_address = line_color;
}

void swap(int* point1, int* point2){
	int temp = *point1;
	*point1 = *point2;
	*point2 = temp;
}

void drawProjectile(Basketball *ball){
    
	// set the initial postion to the ball's position
	int x = test_ball.currentPos.x;
	int y = test_ball.currentPos.y;
	// set the initial velocity of the ball
	int dx = test_ball.currentVel.x;
	int dy = test_ball.currentVel.y;
	
	// draw 7 positions in the projectile path
	for (int deltaTime = x; deltaTime <= 0; deltaTime - 6){
    	// Update the position of the basketball
    	x += dx * deltaTime; // x = v0t
    	y += dy * deltaTime - (0.5 * GRAVITY) * deltaTime * deltaTime; // y = v0t - 0.5gt^2

    	// Update the velocity of the basketball
    	// x velocity remains constant
    	dy -= GRAVITY * deltaTime; // v = v0 - gt
		
		// draw a white box
		draw_box(x, y, 0xFFFF);
	}
}

	
void draw_box(int x, int y, short int color) {
    // Draw box at position (x, y) in back buffer
    // Implement your drawing logic here
    
    // Adjust box size to fit within screen boundaries
    int box_size = 10;
    
    // Ensure x and y are within bounds
    if (x < 0) x = 0;
    if (x + box_size >= 320) x = 320 - box_size - 1;
    if (y < 0) y = 0;
    if (y + box_size >= 240) y = 240 - box_size - 1;

    // Draw box within screen boundaries
    for (int i = 0; i < box_size; i++) {
        for (int j = 0; j < box_size; j++) {
            plot_pixel(x + i, y + j, color);
        }
    }
}

void draw_circle(int x0, int y0, int radius, short int color, bool fill) {
    // Loop through x values from -radius to radius
    for (int x = -radius; x <= radius; x++) {
        // Calculate y for both positive and negative square roots
        int y1 = y0 + sqrt(radius * radius - x * x);
        int y2 = y0 - sqrt(radius * radius - x * x);
        
        // Plot pixels for both halves of the circle
        plot_pixel(x0 + x, y1, color);
        plot_pixel(x0 + x, y2, color);

        // Draw horizontal lines between the upper and lower halves of the circle
		if (fill){
			for (int y = y2 + 1; y < y1; y++) {
				plot_pixel(x0 + x, y, color);
			}
		}
    }
}

// polling for rendering from front buffer done to switch buffers
void wait_for_vsync(){
	volatile int * pixel_ctrl_ptr = (int *) 0xff203020; // base address
	
	*pixel_ctrl_ptr = 1; // start the synchronization process
	// write 1 into front buffer address register
	volatile int * s_bit = (int *)0xFF20302C; // Use unsigned int pointer to read 32 bits
	// polling loop waiting for S bit to go to 0
	while ((*s_bit & 0x1) != 0);
}

