/*| headers |*/
#include <stdint.h>
#include <stddef.h>

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT
#define CONTEXT_SIZE (sizeof(struct samd21_context_t))

/*| types |*/
typedef struct samd21_context_t* context_t;

/*| structures |*/
struct samd21_context_t {
    uint32_t* stack; /* The stack pointer. Must be valid. */
};

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
#define preempt_init()
#define context_switch_first(to)
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
context_init(context_t *const ctx, void (*const fn)(void), 
             uint32_t *const stack_base, const size_t stack_size)
{
    /* This is where we need to put the context.
     * ARM is full-descending stack, so this pointer is to *ctx
     */
    uint32_t* stack_top = stack_base + stack_size - CONTEXT_SIZE;
    *ctx = (context_t)stack_top;
}

/*| public_functions |*/


/*| public_privileged_functions |*/
