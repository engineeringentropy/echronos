#include "samd21-systick.h"

void samd21_systick_init();
void samd21_systick_enable();
void samd21_systick_disable();
void samd21_systick_set_count(uint32_t count);

void samd21_systick_init()
{
    SysTick->CTRL = /* SysTick_CTRL_ENABLE_Msk | */
                    SysTick_CTRL_CLKSOURCE_Msk | /* Use the GCLK_CPU */
                    SysTick_CTRL_TICKINT_Msk; /* Sets SysTick exception to pending */
    /* After reset, the SAMD21 is running OSC8M with /8 - this is approximately 1s between ticks 
     * Use samd21_systick_set_count to set this to a proper value
     */
    SysTick->VAL = 0;
    SysTick->LOAD = 1000000;
}

void samd21_systick_enable()
{
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

void samd21_systick_disable()
{
    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void samd21_systick_set_count(uint32_t count)
{
    if ((count & SAMD21_SYSTICK_MASK) != count) {
        /* This is something of an assert condition? */
    }

    SysTick->LOAD = count & SAMD21_SYSTICK_MASK;
}
