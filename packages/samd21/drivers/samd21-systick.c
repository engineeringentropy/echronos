#include "samd21-systick.h"
#include "samd21-exceptions.h"
#include "rtos-kochab.h"

void samd21_systick_init();
void samd21_systick_enable();
void samd21_systick_disable();
void samd21_systick_set_count(uint32_t count);

extern void exception_preempt_trampoline_systick();

void samd21_systick_init()
{
    SysTick->CTRL = /* SysTick_CTRL_ENABLE_Msk | */
                    SysTick_CTRL_CLKSOURCE_Msk | /* Use the GCLK_CPU */
                    SysTick_CTRL_TICKINT_Msk; /* Sets SysTick exception to pending */
    /* After reset, the SAMD21 is running OSC8M with /8 - this is approximately 10ms between ticks 
     * Use samd21_systick_set_count to set this to a proper value
     */
    SysTick->VAL = 0;
    SysTick->LOAD = 10000;
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

/* This is where the systick comes to */
int tick_irq()
{
    rtos_timer_tick();
    return 1; /* This forces a preemption */
}