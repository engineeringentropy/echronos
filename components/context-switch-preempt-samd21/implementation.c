/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include "samd21-systick.h"

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT
/* Count of the number of registers in the context */
#define SAMD21_CONTEXT_SIZE 16

/*| types |*/
typedef struct samd21_task_context_layout_t* context_t;

/*| structures |*/
/** This is the exception stack and how we add the additional registers to it.
 */

/* The additional parts that need saving for a full context restoration */
struct samd21_exception_additional_t {
    uint32_t R4; /* 0 */
    uint32_t R5; /* 4 */
    uint32_t R6; /* 8 */
    uint32_t R7; /* 12 */
    uint32_t R8; /* 16 */
    uint32_t R9; /* 20 */
    uint32_t R10; /* 24 */
    uint32_t R11; /* 28 */
};

/* On exception entry from an interrupt */
struct samd21_exception_context_stack_t {
    uint32_t R0; /* 32 */
    uint32_t R1; /* 36 */
    uint32_t R2; /* 40 */
    uint32_t R3; /* 44 */
    uint32_t R12; /* 48 */
    uint32_t LR; /* 52 */
    uint32_t PC; /* 56 */
    uint32_t xPSR; /* 60 */
};

struct samd21_task_context_layout_t {
    struct samd21_exception_additional_t high;
    struct samd21_exception_context_stack_t low;
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
    /* Disable all interrupts */
    __disable_irq();
    /* Configure the system timer and enable it */
    samd21_systick_init();
    samd21_systick_enable();
}

static void 
preempt_enable()
{
    __enable_irq();
}

static void
preempt_disable()
{
    __disable_irq();
}

static void
context_init(context_t *const ctx, void (*const fn)(void), 
             uint32_t *const stack_base, const size_t stack_size)
{
    /* This is where we need to put the context.
     * ARM is full-descending stack, so this pointer is to *ctx
     * When we switch to this task for the first time, the stack is reset.
     */
    uint32_t* stack_top = stack_base + stack_size - SAMD21_CONTEXT_SIZE;
    *ctx = (context_t)stack_top;

    struct samd21_task_context_layout_t* context = *ctx;
    context->low.PC = (uint32_t)fn;
}

/*| public_functions |*/


/*| public_privileged_functions |*/
