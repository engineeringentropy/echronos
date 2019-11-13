/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include "samd21-systick.h"

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT
/* Count of the number of registers in the context */
#define SAMD21_CONTEXT_SIZE 17
/* We need to be able to disable pre-emption the first time a thread is switched to.
 * This is because eChronos ASSERTS that pre-emption is disabled on task entry.
 */
#define SAMD21_CONTEXT_FIRST_RUN_BIT 0x1
#define SAMD21_CONTEXT_FIRST_RUN_MASK (~(0x1))

/*| types |*/
typedef uint32_t samd21_run_flags_t;
typedef struct samd21_task_context_layout_t* context_t;

/*| structures |*/
/** This is the exception stack and how we add the additional registers to it.
 */

/* The additional parts that need saving for a full context restoration */
struct samd21_exception_additional_t {
    uint32_t R8;
    uint32_t R9;
    uint32_t R10;
    uint32_t R11;
    /* The run flags are used to disable preemption on the first time through. This is a requirement of KOCHAB. */
    samd21_run_flags_t runFlags;
    uint32_t R4;
    uint32_t R5;
    uint32_t R6;
    uint32_t R7;
};

/* On exception entry from an interrupt */
struct samd21_exception_context_stack_t {
    uint32_t R0; /* 36 */
    uint32_t R1; /* 40 */
    uint32_t R2; /* 44 */
    uint32_t R3; /* 48 */
    uint32_t R12; /* 52 */
    uint32_t LR; /* 56 */
    uint32_t PC; /* 60 */
    uint32_t xPSR; /* 64 */
};

struct samd21_task_context_layout_t {
    struct samd21_exception_additional_t high;
    struct samd21_exception_context_stack_t low;
};

/*| extern_declarations |*/
extern void rtos_internal_context_switch_first(context_t *);
extern void rtos_internal_task_entry_trampoline(void);
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
static uint8_t 
rtos_internal_check_preempt_disabled(void)
{
    uint32_t primask = __get_PRIMASK();
    /* if primask & 0x1, then interrupts are disabled. */
    if (primask & 1) {
        return 1;
    }
    
    /* Otherwise, if we're in an exception, we need to check its level 
     * We mask this, which means that the range is definitely not negative.
     */
    int ipsr = __get_IPSR() & 0x3F;
    if (0 == ipsr) {
        /* Pre-emption is not disabled, because we're in thread mode and not an exception. */
        return 0;
    }

    /* Because of the way the Cortex-M0+ works, with PendSV set to a priority level of 3
     *  we can't be interrupted at this point
     */
    return 1;
}

static void
preempt_init()
{
    /* Disable all interrupts */
    __disable_irq();

    /* Configure the priorities of SysTick, PendSV, SVC
     * Note that SysTick must always be able to execute, so it has priority 0
     * SVC comes after that, and PendSV is the final one.
     * NVIC_SetPriority takes 0-3, but note that this is converted to 0-192 internally
     * Lower is higher priority
     */
    NVIC_SetPriority(SysTick_IRQn, 0);
    NVIC_SetPriority(SVCall_IRQn, 1);
    /* Other interrupts are priority 2? */
    NVIC_SetPriority(PendSV_IRQn, 3);
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
    context->low.xPSR = (uint32_t)1 << 24;
    context->high.runFlags = SAMD21_CONTEXT_FIRST_RUN_BIT;
}

/*| public_functions |*/


/*| public_privileged_functions |*/
