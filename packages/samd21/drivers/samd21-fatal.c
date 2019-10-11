#include "samd21-systick.h"
#include "samd21.h"

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
    samd21_systick_disable();
    __disable_irq();
    while(1) {
        fatal_delay(1);
    }
}