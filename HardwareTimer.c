// Sample code for hardware timer 1 second delay that uses polling

void waitASec();

struct timer_t {
    volatile unsigned int status;
    volatile unsigned int control;
    volatile unsigned int periodlo;
    volatile unsigned int periodhi;
    volatile unsigned int snaplo;
    volatile unsigned int snaphi;
};

struct timer_t *timer = (struct timer_t *)0xFF202000;

#define TIMERSEC 100000000

void waitASec() {
       timer->control = 0x8; // stop the timer
       timer->status = 0; // reset TO
       timer->periodlo = (TIMERSEC & 0x0000FFFF);
       timer->periodhi = (TIMERSEC & 0xFFFF0000) >> 16;
       timer->control = 0x4;
       while ((timer->status & 0x1) == 0);
       timer->status = 0; // reset TO
       printf("1 second has passed\n");
}

int main() {
    while (1) {
        waitASec();
    }
    return 0;
}