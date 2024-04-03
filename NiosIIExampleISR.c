/*******************************************************************************
 * This program demonstrates use of interrupts. It
 * first starts the interval timer with 50 msec timeouts, and then enables
 * Nios II interrupts from the interval timer and pushbutton KEYs
 *
 * The interrupt service routine for the interval timer displays a pattern on
 * the LED lights, and shifts this pattern either left or right. The shifting
 * direction is reversed when KEY[1] is pressed
 ********************************************************************************/

/* address_map_nios2.h */
/*******************************************************************************
 * This file provides address values that exist in the DE1-SoC Computer
 ******************************************************************************/

#ifndef __SYSTEM_INFO__
#define __SYSTEM_INFO__

#define BOARD "DE1-SoC"

/* Memory */
#define DDR_BASE 0x40000000
#define DDR_END 0x7FFFFFFF
#define A9_ONCHIP_BASE 0xFFFF0000
#define A9_ONCHIP_END 0xFFFFFFFF
#define SDRAM_BASE 0x00000000
#define SDRAM_END 0x03FFFFFF
#define FPGA_PIXEL_BUF_BASE 0x08000000
#define FPGA_PIXEL_BUF_END 0x0803FFFF
#define FPGA_CHAR_BASE 0x09000000
#define FPGA_CHAR_END 0x09001FFF

/* Cyclone V FPGA devices */
#define LED_BASE 0xFF200000
#define LEDR_BASE 0xFF200000
#define HEX3_HEX0_BASE 0xFF200020
#define HEX5_HEX4_BASE 0xFF200030
#define SW_BASE 0xFF200040
#define KEY_BASE 0xFF200050
#define JP1_BASE 0xFF200060
#define JP2_BASE 0xFF200070
#define PS2_BASE 0xFF200100
#define PS2_DUAL_BASE 0xFF200108
#define JTAG_UART_BASE 0xFF201000
#define IrDA_BASE 0xFF201020
#define TIMER_BASE 0xFF202000
#define TIMER_2_BASE 0xFF202020
#define AV_CONFIG_BASE 0xFF203000
#define RGB_RESAMPLER_BASE 0xFF203010
#define PIXEL_BUF_CTRL_BASE 0xFF203020
#define CHAR_BUF_CTRL_BASE 0xFF203030
#define AUDIO_BASE 0xFF203040
#define VIDEO_IN_BASE 0xFF203060
#define EDGE_DETECT_CTRL_BASE 0xFF203070
#define ADC_BASE 0xFF204000

/* Cyclone V HPS devices */
#define HPS_GPIO1_BASE 0xFF709000
#define I2C0_BASE 0xFFC04000
#define I2C1_BASE 0xFFC05000
#define I2C2_BASE 0xFFC06000
#define I2C3_BASE 0xFFC07000
#define HPS_TIMER0_BASE 0xFFC08000
#define HPS_TIMER1_BASE 0xFFC09000
#define HPS_TIMER2_BASE 0xFFD00000
#define HPS_TIMER3_BASE 0xFFD01000
#define FPGA_BRIDGE 0xFFD0501C

#endif

/* nios2_ctrl_reg_macros.h */
#ifndef __NIOS2_CTRL_REG_MACROS__
#define __NIOS2_CTRL_REG_MACROS__
/*****************************************************************************/
/* Macros for accessing the control registers. */
/*****************************************************************************/
#define NIOS2_READ_STATUS(dest)    \
    do {                           \
        dest = __builtin_rdctl(0); \
    } while (0)
#define NIOS2_WRITE_STATUS(src)  \
    do {                         \
        __builtin_wrctl(0, src); \
    } while (0)
#define NIOS2_READ_ESTATUS(dest)   \
    do {                           \
        dest = __builtin_rdctl(1); \
    } while (0)
#define NIOS2_READ_BSTATUS(dest)   \
    do {                           \
        dest = __builtin_rdctl(2); \
    } while (0)
#define NIOS2_READ_IENABLE(dest)   \
    do {                           \
        dest = __builtin_rdctl(3); \
    } while (0)
#define NIOS2_WRITE_IENABLE(src) \
    do {                         \
        __builtin_wrctl(3, src); \
    } while (0)
#define NIOS2_READ_IPENDING(dest)  \
    do {                           \
        dest = __builtin_rdctl(4); \
    } while (0)
#define NIOS2_READ_CPUID(dest)     \
    do {                           \
        dest = __builtin_rdctl(5); \
    } while (0)
#endif

/* globals.h */
#define LEFT 0
#define RIGHT 1
#define ENABLE 1
#define DISABLE 0

/* the global variables are written by interrupt service routines; we have to
 * declare
 * these as volatile to avoid the compiler caching their values in registers */
volatile int pattern = 0x0000000F;   // pattern for shifting
volatile int shift_dir = LEFT;       // direction to shift the pattern
volatile int shift_enable = ENABLE;  // enable/disable shifting of the pattern

/* keyboard bytes */
volatile int byte1 = 0;
volatile int byte2 = 0;
volatile int byte3 = 0;

/* function prototypes */
int main(void);
void interrupt_handler(void);
void interval_timer_ISR(void);
void pushbutton_ISR(void);

/* The assembly language code below handles CPU reset processing */
void the_reset(void) __attribute__((section(".reset")));
void the_reset(void)
/*******************************************************************************
 * Reset code. By giving the code a section attribute with the name ".reset" we
 * allow the linker program to locate this code at the proper reset vector
 * address. This code just calls the main program.
 ******************************************************************************/
{
    asm(".set noat");       /* Instruct the assembler NOT to use reg at (r1) as
                             * a temp register for performing optimizations */
    asm(".set nobreak");    /* Suppresses a warning message that says that
                             * some debuggers corrupt regs bt (r25) and ba
                             * (r30)
                             */
    asm("movia r2, main");  // Call the C language main program
    asm("jmp r2");
}

/* The assembly language code below handles CPU exception processing. This
 * code should not be modified; instead, the C language code in the function
 * interrupt_handler() can be modified as needed for a given application.
 */
void the_exception(void) __attribute__((section(".exceptions")));
void the_exception(void)
/*******************************************************************************
 * Exceptions code. By giving the code a section attribute with the name
 * ".exceptions" we allow the linker program to locate this code at the proper
 * exceptions vector address.
 * This code calls the interrupt handler and later returns from the exception.
 ******************************************************************************/
{
    asm("subi sp, sp, 128");
    asm("stw et, 96(sp)");
    asm("rdctl et, ctl4");
    asm("beq et, r0, SKIP_EA_DEC");  // Interrupt is not external
    asm("subi ea, ea, 4");           /* Must decrement ea by one instruction
                                      * for external interupts, so that the
                                      * interrupted instruction will be run */
    asm("SKIP_EA_DEC:");
    asm("stw r1, 4(sp)");  // Save all registers
    asm("stw r2, 8(sp)");
    asm("stw r3, 12(sp)");
    asm("stw r4, 16(sp)");
    asm("stw r5, 20(sp)");
    asm("stw r6, 24(sp)");
    asm("stw r7, 28(sp)");
    asm("stw r8, 32(sp)");
    asm("stw r9, 36(sp)");
    asm("stw r10, 40(sp)");
    asm("stw r11, 44(sp)");
    asm("stw r12, 48(sp)");
    asm("stw r13, 52(sp)");
    asm("stw r14, 56(sp)");
    asm("stw r15, 60(sp)");
    asm("stw r16, 64(sp)");
    asm("stw r17, 68(sp)");
    asm("stw r18, 72(sp)");
    asm("stw r19, 76(sp)");
    asm("stw r20, 80(sp)");
    asm("stw r21, 84(sp)");
    asm("stw r22, 88(sp)");
    asm("stw r23, 92(sp)");
    asm("stw r25, 100(sp)");  // r25 = bt (skip r24 = et, because it is saved
    // above)
    asm("stw r26, 104(sp)");  // r26 = gp
    // skip r27 because it is sp, and there is no point in saving this
    asm("stw r28, 112(sp)");  // r28 = fp
    asm("stw r29, 116(sp)");  // r29 = ea
    asm("stw r30, 120(sp)");  // r30 = ba
    asm("stw r31, 124(sp)");  // r31 = ra
    asm("addi fp, sp, 128");
    asm("call interrupt_handler");  // Call the C language interrupt handler
    asm("ldw r1, 4(sp)");           // Restore all registers
    asm("ldw r2, 8(sp)");
    asm("ldw r3, 12(sp)");
    asm("ldw r4, 16(sp)");
    asm("ldw r5, 20(sp)");
    asm("ldw r6, 24(sp)");
    asm("ldw r7, 28(sp)");
    asm("ldw r8, 32(sp)");
    asm("ldw r9, 36(sp)");
    asm("ldw r10, 40(sp)");
    asm("ldw r11, 44(sp)");
    asm("ldw r12, 48(sp)");
    asm("ldw r13, 52(sp)");
    asm("ldw r14, 56(sp)");
    asm("ldw r15, 60(sp)");
    asm("ldw r16, 64(sp)");
    asm("ldw r17, 68(sp)");
    asm("ldw r18, 72(sp)");
    asm("ldw r19, 76(sp)");
    asm("ldw r20, 80(sp)");
    asm("ldw r21, 84(sp)");
    asm("ldw r22, 88(sp)");
    asm("ldw r23, 92(sp)");
    asm("ldw r24, 96(sp)");
    asm("ldw r25, 100(sp)");  // r25 = bt
    asm("ldw r26, 104(sp)");  // r26 = gp
    // skip r27 because it is sp, and we did not save this on the stack
    asm("ldw r28, 112(sp)");  // r28 = fp
    asm("ldw r29, 116(sp)");  // r29 = ea
    asm("ldw r30, 120(sp)");  // r30 = ba
    asm("ldw r31, 124(sp)");  // r31 = ra
    asm("addi sp, sp, 128");
    asm("eret");
}

/******************************************************************************
 * Interrupt Service Routine
 * Determines what caused the interrupt and calls the appropriate
 * subroutine.
 *
 * ipending - Control register 4 which has the pending external interrupts
 ******************************************************************************/
void interrupt_handler(void) {
    int ipending;
    NIOS2_READ_IPENDING(ipending);
    if (ipending & 0x1)  // interval timer is interrupt level 0
    {
        interval_timer_ISR();
    }
    if (ipending & 0x2)  // pushbuttons are interrupt level 1
    {
        pushbutton_ISR();
    }
    if (ipending & 0x7) // PS/2 is interrupt level 7
    {
        ps2_ISR();
    }
    // else, ignore the interrupt
    return;
}

/*******************************************************************************
 * Interval timer interrupt service routine
 *
 * Shifts a PATTERN being displayed on the LED lights. The shift direction
 * is determined by the external variable key_dir.
 ******************************************************************************/
void interval_timer_ISR() {
    volatile int *interval_timer_ptr = (int *)TIMER_BASE;
    volatile int *LEDG_ptr = (int *)LED_BASE;  // LED address

    *(interval_timer_ptr) = 0;                 // clear the interrupt
    *(LEDG_ptr) = pattern;                     // display pattern on LED
    if (shift_enable == DISABLE)               // check if shifting is disabled
        return;
    /* rotate the pattern shown on the LEDG lights */
    if (shift_dir == LEFT)  // rotate left
        if (pattern & 0x80000000)
            pattern = (pattern << 1) | 1;
        else
            pattern = pattern << 1;
    else  // rotate right
        if (pattern & 0x00000001)
            pattern = (pattern >> 1) | 0x80000000;
        else
            pattern = (pattern >> 1) & 0x7FFFFFFF;
    return;
}

/*******************************************************************************
 * Pushbutton - Interrupt Service Routine
 *
 * This routine checks which KEY has been pressed and updates the global
 * variables as required.
 ******************************************************************************/
void pushbutton_ISR(void) {
    printf("Pushbutton ISR\n");
    volatile int *KEY_ptr = (int *)KEY_BASE;
    volatile int *slider_switch_ptr = (int *)SW_BASE;
    int press;
    press = *(KEY_ptr + 3);  // read the pushbutton interrupt register
    *(KEY_ptr + 3) = press;  // Clear the interrupt
    if (press & 0x1)         // KEY0
        pattern = *slider_switch_ptr;
    if (press & 0x2)  // KEY1
        shift_dir = shift_dir ^ 1;
    return;
}

/*******************************************************************************
 * PS/2 - Interrupt Service Routine
 *
 * This routine checks which KEY has been pressed and updates the global
 * variables as required. Left arrow key is for left shift, right arrow key is for right shift.
 * 
 ******************************************************************************/

void ps2_ISR(void) {
    printf("PS2 ISR\n");
    volatile int *PS2_ptr = (int *)PS2_BASE;
    int PS2_data, RVALID;
    PS2_data = *(PS2_ptr);  // read the Data register in the PS/2 port

    // clear the interrupt
    *(PS2_ptr + 1) = 0;

    RVALID = PS2_data & 0x8000;  // extract the RVALID field
    if (RVALID) {
        /* shift the next data byte into the display */
        byte1 = byte2;
        byte2 = byte3;
        byte3 = PS2_data & 0xFF;
        if (byte2 == 0xF0)  // key release
        {
            if (byte3 == 0x6B)  // left arrow key
                shift_dir = LEFT;
            if (byte3 == 0x74)  // right arrow key
                shift_dir = RIGHT;
        }
    }

    return;
}

int main(void) {
    /* Declare volatile pointers to I/O registers (volatile means that IO load
     * and store instructions will be used to access these pointer locations,
     * instead of regular memory loads and stores)
     */
    volatile int *interval_timer_ptr = (int *)TIMER_BASE;  // interal timer base address
    volatile int *KEY_ptr = (int *)KEY_BASE;               // pushbutton KEY address
    volatile int *PS2_ptr = (int *)PS2_BASE;               // PS/2 port address
    /* set the interval timer period for scrolling the LED lights */
    int counter = 2500000;  // 1/(50 MHz) x (2500000) = 50 msec
    *(interval_timer_ptr + 0x2) = (counter & 0xFFFF);
    *(interval_timer_ptr + 0x3) = (counter >> 16) & 0xFFFF;
    /* start interval timer, enable its interrupts */
    *(interval_timer_ptr + 1) = 0x7;  // STOP = 0, START = 1, CONT = 1, ITO = 1
    *(KEY_ptr + 2) = 0x3;             // enable interrupts for all pushbuttons
    *(PS2_ptr + 8) = 0x1;             // enable interrupts for PS/2 port
    // /* set interrupt mask bits for levels 0 (interval timer) and level 1 and level 4 (keyboard)
    //  * (pushbuttons) */
    // NIOS2_WRITE_IENABLE(0x3);

    // enable interrupts for levels 0, 1, and 7
    NIOS2_WRITE_IENABLE(0x83);
    NIOS2_WRITE_STATUS(1);  // enable Nios II interrupts
    while (1)
        ;  // main program simply idles
}