/*

 */
#include "sam.h"
#include "rtos-kochab.h"

/* Ticks occur at 100Hz */
const int tickMs = 10;
/* This is approximately 25ms in tickMs units */
const int delayTicks = 25;

extern void fatal();

uint32_t function1()
{
    return 25;
}

void thread1(void)
{
    while (1) {
        rtos_sleep(delayTicks);
        if (function1() != 25)
        {
            fatal();
        }
        rtos_sleep(delayTicks);
    }
}

uint32_t function2()
{
    return 75;
}

void thread2(void)
{
    while (1) {
        rtos_sleep(delayTicks);
        if (function2() != 75)
        {
            fatal();
        }
        rtos_sleep(delayTicks);
    }
}
