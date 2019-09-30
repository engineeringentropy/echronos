#include "samd21.h"
#include "rtos-gatria.h"

/* Roughly (very roughly) 1ms/n */
void delay(int n)
{
    int i;

    for (;n >0; n--)
    {
        for (i=0;i<100;i++)
            __asm("nop");
    }

    /* Give other threads an opportunity to run */
    rtos_yield();
}

void thread1(void)
{
    /* Do thread intialisation */
    REG_PORT_DIR0 |= (1 << 6);
    
    while (1) {
        REG_PORT_OUT0 &= ~(1 << 6);
        delay(250);
        REG_PORT_OUT0 |= (1 << 6);
        delay(250);
    }
}

void thread2(void)
{
    /* Do thread initialisation */
    REG_PORT_DIR0 |= (1 << 17);

    while (1) {
        REG_PORT_OUT0 |= (1 << 17);
        delay(250);
        REG_PORT_OUT0 &= ~(1 << 17);
        delay(250);
    }
}
