/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include "samd21-systick.h"

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT
#define CONTEXT_SIZE (sizeof(struct samd21_context_t))

/*| types |*/
typedef struct samd21_context_t* context_t;

/*| structures |*/
/** This is the exception stack and how we add the additional registers to it.
 */
struct samd21_context_stack_t {
    uint32_t xPSR;
};

struct samd21_context_t {
    uint32_t* stack; /* The stack pointer. Must be valid. */
};

/*| extern_declarations |*/
extern void rtos_internal_context_switch_first(context_t *);
extern void rtos_internal_task_entry_trampoline(void);
extern uint8_t rtos_internal_check_preempt_disabled(void);
extern void rtos_internal_yield(void);
extern void rtos_internal_preempt_enable(void);
extern void rtos_internal_preempt_disable(void);
extern void rtos_internal_preempt_pend(void);

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
#define context_switch_first(to) rtos_internal_context_switch_first(get_task_context(to))
#define yield() rtos_internal_yield()
#define preempt_disable() rtos_internal_preempt_disable()
#define preempt_enable() rtos_internal_preempt_enable()
#define preempt_pend() rtos_internal_preempt_pend()
#define precondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_PRECONDITION_VIOLATED)
#define postcondition_preemption_disabled() internal_assert(rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)
#define postcondition_preemption_enabled() internal_assert(!rtos_internal_check_preempt_disabled(), \
        ERROR_ID_INTERNAL_POSTCONDITION_VIOLATED)

/*| functions |*/
static void
preempt_init()
{
    /* Turn on the systick and enable it */
    samd21_systick_init();
    samd21_systick_enable();
}

static void
context_init(context_t *const ctx, void (*const fn)(void), 
             uint32_t *const stack_base, const size_t stack_size)
{
    /* This is where we need to put the context.
     * ARM is full-descending stack, so this pointer is to *ctx
     * When we switch to this task for the first time, the stack is reset.
     */
    uint32_t* stack_top = stack_base + stack_size - CONTEXT_SIZE;
    *ctx = (context_t)stack_top;
}

/*| public_functions |*/


/*| public_privileged_functions |*/
