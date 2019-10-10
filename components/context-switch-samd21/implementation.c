/*| headers |*/
#include <stdint.h>

/*| object_like_macros |*/
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
extern void rtos_internal_context_switch(context_t *, context_t *) {{prefix_const}}REENTRANT;
extern void rtos_internal_context_switch_first(context_t *) {{prefix_const}}REENTRANT;

/*| function_declarations |*/

/*| state |*/

/*| function_like_macros |*/
#define context_switch(from, to) rtos_internal_context_switch(to, from)
#define context_switch_first(to) rtos_internal_context_switch_first(to)

/*| functions |*/
static void
context_init(context_t *const ctx, void (*const fn)(void), 
             uint32_t *const stack_base, const size_t stack_size)
{
    uint32_t *context;
    context = stack_base + stack_size - CONTEXT_SIZE;
    
    /* We need only fill in PC, as the other parts should be initialised by the callee */
    context[SAMD21_CONTEXT_PC] = (uint32_t)fn;

    *ctx = context;
}

/*| public_functions |*/

/*| public_privileged_functions |*/
