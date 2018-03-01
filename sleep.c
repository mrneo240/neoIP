#include "neoIP.h"

/* Quick access macros */
#define TIMER8(o) ( *((volatile uint8_t*)(0xffd80000 + (o))) )
#define TIMER16(o) ( *((volatile uint16_t*)(0xffd80000 + (o))) )
#define TIMER32(o) ( *((volatile uint32_t*)(0xffd80000 + (o))) )
#define TOCR    0x00
#define TSTR    0x04
#define TCOR0   0x08
#define TCNT0   0x0c
#define TCR0    0x10
#define TCOR1   0x14
#define TCNT1   0x18
#define TCR1    0x1c
#define TCOR2   0x20
#define TCNT2   0x24
#define TCR2    0x28
#define TCPR2   0x2c
#define TMU1    1

static int tcors[] = { TCOR0, TCOR1, TCOR2 };
static int tcnts[] = { TCNT0, TCNT1, TCNT2 };
static int tcrs[] = { TCR0, TCR1, TCR2 };

/* Disable timer interrupts; needs to move to irq.c sometime. */
void timer_disable_ints(int which) {
    volatile uint16_t *ipra = (uint16_t*)0xffd00004;
    *ipra &= ~(0x000f << (12 - 4 * which));
}

/* Pre-initialize a timer; set values but don't start it */
int timer_prime(int which, uint32_t speed, int interrupts) {
    /* P0/64 scalar, maybe interrupts */
    if(interrupts)
        TIMER16(tcrs[which]) = 32 | 2;
    else
        TIMER16(tcrs[which]) = 2;

    /* Initialize counters; formula is P0/(tps*64) */
    TIMER32(tcnts[which]) = 781;//50000000 / (speed * 64);
    TIMER32(tcors[which]) = 781;//50000000 / (speed * 64);

    //if(interrupts)
    //    timer_enable_ints(which);

    return 0;
}

/* Start a timer -- starts it running (and interrupts if applicable) */
int timer_start(int which) {
    TIMER8(TSTR) |= 1 << which;
    return 0;
}

/* Stop a timer -- and disables its interrupt */
int timer_stop(int which) {
    timer_disable_ints(which);

    /* Stop timer */
    TIMER8(TSTR) &= ~(1 << which);

    return 0;
}
/* Clears the timer underflow bit and returns what its value was */
int timer_clear(int which) {
    uint16_t value = TIMER16(tcrs[which]);
    TIMER16(tcrs[which]) &= ~0x100;

    return (value & 0x100) ? 1 : 0;
}

void timer_spin_sleep(int ms) {
    timer_prime(TMU1, 1000, 0);
    timer_clear(TMU1);
    timer_start(TMU1);

    while(ms > 0) {
        while(!(TIMER16(tcrs[TMU1]) & 0x100))
            ;

        timer_clear(TMU1);
        ms--;
    }

    timer_stop(TMU1);
}

void thd_sleep(int ms) {
    timer_spin_sleep(ms);
}

unsigned int sleep(unsigned int seconds) {
    thd_sleep(seconds * 1000);
    return 0;
}
unsigned int sleep_ms(unsigned int ms){
	thd_sleep(ms);
 return 0;
}