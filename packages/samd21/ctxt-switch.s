/* Context switch for AT91SAM21G18A
 *
 * Note that because this is cooperative multitasking, the calling convention
 *  requires that functions preserve R4-R15, but are free to trash R0-3.
 * This means that our task context consists only of R4-R15!
 * Note that R13 is the SP (and can't be saved in a push), and R15 is the PC
 *  which needs saving in a different way!
 */

.global rtos_internal_context_switch
/* Prototype: rtos_internal_context_switch(uint32_t** to, uint32_t** from) */
rtos_internal_context_switch:
    /* Save the state of the processor
     * Not required to preserve R0-R3 because we're in a function call
     */
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
    /* Deliberately fall through to part 2, which is the restoration */

.global rtos_internal_context_switch_first
rtos_internal_context_switch_first:
    /* Not required to preserve R0-R3 */
    ldr r7, [r0]
    mov sp, r7
    pop {r7}
    mov r12, r7
    pop {r4-r7}
    mov r8, r4 
    mov r9, r5
    mov r10, r6
    mov r11, r7
    pop {r4-r7, PC}
