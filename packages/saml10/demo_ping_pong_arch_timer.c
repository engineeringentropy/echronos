#include "sam.h"
#include "rtos-kochab.h"

/* Ticks occur at 100Hz */
const int tickMs = 10;
/* This is approximately 25ms in tickMs units */
const int delayTicks = 25;

void thread1(void)
{
    /* Do thread intialisation */
    REG_PORT_DIR0 |= (1 << 6);
    
    while (1) {
        REG_PORT_OUT0 |= (1 << 6);
        rtos_sleep(delayTicks);
        REG_PORT_OUT0 &= ~(1 << 6);
        rtos_sleep(delayTicks);
    }
}

void thread2(void)
{
    /* Do thread initialisation */
    REG_PORT_DIR0 |= (1 << 17);

    while (1) {
        REG_PORT_OUT0 |= (1 << 17);
        rtos_sleep(delayTicks);
        REG_PORT_OUT0 &= ~(1 << 17);
        rtos_sleep(delayTicks);
    }
}
