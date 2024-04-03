// Sample code for hardware timer 1 second delay

struct timer_t {
    volatile unsigned int status;
    volatile unsigned int control;
    volatile unsigned int periodlo;
    volatile unsigned int periodhi;
    volatile unsigned int snaplo;
    volatile unsigned int snaphi;
};

struct timer_t *timer = (struct timer_t *)0xFF202000;

#define TIMERSEC 1000000

void waitASec() {
    timer->control = 0x8;
    timer->status = 0x0; // Reset TO
    timer->periodlo = TIMERSEC & 0xFFFF;
    timer->periodhi = TIMERSEC && 0xFFFF0000 >> 16;
    timer->control = 0x4;
    while ((timer->status & 0x1) == 0);
    printf("A second has passed");
    timer->status = 0x0;
}

int main() {
    while (1) {
        waitASec();
    }
    return 0;
}