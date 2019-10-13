/* Note: Some of this is cribbed from the ARMv7 architecture, but the ARMv6 is quite different in the 
 *   way that exceptions are handled.
 */

/* This macro invokes the scheduler in order to determine the TaskId of the next task to be switched to.
 * The TaskId of the task to be switched to ends up in r0. */
.macro asm_invoke_scheduler
        /* new_task<r0> = interrupt_event_get_next() */
        push {ip, lr}
        bl rtos_internal_interrupt_event_get_next
        pop {ip, lr}
.endm

/* This macro retrieves the address and current value of the 'rtos_internal_current_task' variable.
 * The pointer to the 'rtos_internal_current_task' variable is placed into the 'current_task_p' register, and the
 * current value of the variable itself is placed into the 'old_task' register. */
.macro asm_current_task_get current_task_p old_task
        /* old_task = rtos_internal_current_task */
        ldr \current_task_p, =rtos_internal_current_task
        ldrb \old_task, [\current_task_p]
.endm

/* This function returns 1 if preemption is disabled, and 0 otherwise.
 */
.global rtos_internal_check_preempt_disabled
/* bool rtos_internal_check_preempt_disabled(void); */
rtos_internal_check_preempt_disabled:
        mrs r0, primask
        movs r1, #1
        and r0, r0, r1
        bx lr

/**
 * Trigger a context-switch to the next task runnable as determined by the scheduler.
 * Intended to be used by the RTOS internally after taking actions that change the set of schedulable tasks.
 */
.global rtos_internal_yield
/* void rtos_internal_yield(void); */
rtos_internal_yield:
        /* We implement manual context switch using ARM's SVC (supervisor call) exception.
         * Upon executing the 'svc' instruction, the CPU immediately takes a SVC exception and jumps to
         * 'rtos_internal_svc_handler'. */
        svc #0
        bx lr

/*
@ The additional parts that need saving for a full context restoration
@ struct samd21_exception_additional_t {
@     uint32_t R4; /* 0 */
@     uint32_t R5; /* 4 */
@     uint32_t R6; /* 8 */
@     uint32_t R7; /* 12 */
@     uint32_t R8; /* 16 */
@     uint32_t R9; /* 20 */
@     uint32_t R10; /* 24 */
@     uint32_t R11; /* 28 */
@ };

@ /* On exception entry from an interrupt */
@ struct samd21_exception_context_stack_t {
@     uint32_t R0; /* 32 */
@     uint32_t R1; /* 36 */
@     uint32_t R2; /* 40 */
@     uint32_t R3; /* 44 */
@     uint32_t R12; /* 48 */
@     uint32_t LR; /* 52 */
@     uint32_t PC; /* 56 */
@     uint32_t xPSR; /* 60 */
@ };

@ struct samd21_task_context_layout_t {
@     struct samd21_exception_additional_t high;
@     struct samd21_exception_context_stack_t low;
@ };

@ struct samd21_context_t {
@     struct samd21_task_context_layout_t* stack;
@ };
@ void rtos_internal_context_switch_first(context_t *);
@ */
.global rtos_internal_context_switch_first
rtos_internal_context_switch_first:
        /* On entry, we're on the main stack and in thread mode, so set the new stack and switch the stack
         * We don't need to set LR or anything else.
         * Interrupts are disable here, we don't enable them.
         * Interrupts are enabled during the entry_{task} function.
         */
        /* Put the stack pointer into psp */
        ldr r1, [r0]
        /* Reset the stack by the full size of the context */
        add r1, #64
        msr psp, r1
        /* #56 comes from samd21_exception_context_stack_t */
        ldr r1, [r0,#56]
        /* Clean up our stack to the top
         * Note that this is the size of the SP in bytes!
         * This must be the same as MSPStack in samd21-exceptions.c
         * This is legal because rtos_start doesn't return.
         * Transitively, this means that this function does not return.
         */
        add sp, #512
        /* Switch stacks */
        mrs r2, control
        or r2, #2
        msr control, r2
        /* Start executing the task */
        mov pc, r1

.global rtos_internal_pendsv_handler
rtos_internal_pendsv_handler:
        

.global rtos_tick_irq
rtos_tick_irq:
        /* We get here from exception_preempt_trampoline_systick,
         *  which is done on the MSP.
         * We need to save the additional registers on the PSP.
         * We can't be preempted to get back here
         */