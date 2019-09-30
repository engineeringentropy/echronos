/*| headers |*/
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/*| object_like_macros |*/
#define PREEMPTION_SUPPORT

/* This defines the layout of the context stack
    push {r4-r7, LR}
    mov r4, r8
    mov r5, r9
    mov r6, r10
    mov r7, r11
    push {r4-r7}
    mov r7, r12
    push {r7}
    mov r7, sp
    str r7, [r1]
*/
#define SAMD21_CONTEXT_PC  9
#define SAMD21_CONTEXT_R7  8
#define SAMD21_CONTEXT_R6  7
#define SAMD21_CONTEXT_R5  6
#define SAMD21_CONTEXT_R4  5
#define SAMD21_CONTEXT_R11 4
#define SAMD21_CONTEXT_R10 3
#define SAMD21_CONTEXT_R9  2
#define SAMD21_CONTEXT_R8  1
#define SAMD21_CONTEXT_R12 0
#define CONTEXT_SIZE 10

/*| types |*/
typedef uint32_t* context_t;

/*| structures |*/

/*| extern_declarations |*/

/*| function_declarations |*/

/*| state |*/
void (*entry_point_ptr)(void);

/*| function_like_macros |*/
#define preempt_init()
#define context_init(ctx, fn, stack_base, stack_size) (entry_point_ptr = fn)
#define context_switch_first(to)
#define yield()
#define preempt_disable()
#define preempt_enable()
#define preempt_pend()
#define precondition_preemption_disabled()
#define postcondition_preemption_disabled()
#define postcondition_preemption_enabled()

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), 
             uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t* context = stack_base + stack_size - CONTEXT_SIZE;
    
    /* We need only fill in PC, as the other parts should be initialised by the callee */
    context[SAMD21_CONTEXT_PC] = (uint32_t)fn;

    *ctx = context;
}

/*| public_functions |*/
/* This is used to reduce the overhead of tasks and their initial switches.
 * The problem is that GCC uses the Procedure Call Reference for ARM (by ARM)
 *   which generates a function call that preserves a bunch of registers.
 * "A subroutine must preserve the contents of the registers r4-r8, r10, r11 and SP 
 *      (and r9 in PCS variants that designate r9 as v6)."
 * We have no idea which registers GCC has chosen to preserve, so we just reset the stack
 *    for the current task.
 * This function is re-entrant.
 */
static void __taskStackReset() REENTRANT
{
    
}

/*| public_privileged_functions |*/
