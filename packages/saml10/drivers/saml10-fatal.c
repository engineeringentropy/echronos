#include "sam.h"

/* Roughly (very roughly) 1ms/n */
static void fatal_delay(int n)
{
    int i;

    for (;n >0; n--)
    {
        for (i=0;i<100;i++)
            __asm("nop");
    }
}

/* Fatal driver.
 * Stops systick and disasbles interrupts.
 * Interrupts should already be disabled when we get here, but worth checking.
 */

__attribute__((noreturn))
void fatal()
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
    __disable_irq();
    while(1) {
        fatal_delay(1);
    }
}