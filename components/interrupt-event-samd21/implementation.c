/* Note: Mostly cribbed from the POSIX implementation, as the BITBAND implementation doesn't 
 *  exist in the CortexM0+ processor we're interested in.
 *       Some parts are cribbed from the ARMv7 implementation.
 */

/*| headers |*/
#include <stdint.h>
#include "samd21.h"

/*| object_like_macros |*/

/*| types |*/

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/
static void interrupt_event_process(void);
static inline void interrupt_event_wait(void);
static inline uint32_t interrupt_application_event_check(void);

/*| state |*/
{{#interrupt_events.length}}
static uint32_t pending_interrupt_events;
{{/interrupt_events.length}}

/*| function_like_macros |*/

/*| functions |*/
/* This is called as an 'idle task' in the rtos_internal_interrupt_event_get_next function.
 */
static inline void
interrupt_event_wait(void)
{
    __disable_irq();
    asm volatile("isb");
    if (!interrupt_event_check())
    {
        asm volatile("wfi");
    }
    __enable_irq();
}

static inline uint32_t
interrupt_application_event_check(void)
{
{{#interrupt_events.length}}
    return pending_interrupt_events != 0;
{{/interrupt_events.length}}
{{^interrupt_events.length}}
    return 0;
{{/interrupt_events.length}}
}

static void
interrupt_event_process(void)
{
{{#interrupt_events.length}}
    uint32_t tmp = pending_interrupt_events;
    uint32_t count = 0;
    uint32_t mask = 1;
    while (tmp != 0) {
        if (tmp & mask) {
            /*
             * We're about to handle an interrupt. Because we're about to handle it, we set it to 0 in pending_interrupt_events.
             * This should be terrifying - if we have an interrupt interrupt us, we could easily end up corrupted.
             * So we disable interrupts by becoming the highest priority in the system for a few moments.
             * We use XOR here because we know that the bit is set, and that an interrupt only ever SETS bits, not clears them.
             * If the previously line changes at some point, this is no longer valid.
             */
            uint32_t primask = __get_PRIMASK() & 1;
            __disable_irq();
            pending_interrupt_events ^= mask;
            if (0 == primask)
            {
                __enable_irq();
            }

            interrupt_event_handle(count);
        }
        tmp ^= mask;
        pending_interrupt_events ^= mask;
        mask <<= 1;
        count++;
    }
{{/interrupt_events.length}}
}

/*| public_functions |*/
{{#interrupt_events.length}}
void
{{prefix_func}}interrupt_event_raise(const {{prefix_type}}InterruptEventId interrupt_event_id)
{
    pending_interrupt_events = pending_interrupt_events | (1 << interrupt_event_id);
}
{{/interrupt_events.length}}

/*| public_privileged_functions |*/
