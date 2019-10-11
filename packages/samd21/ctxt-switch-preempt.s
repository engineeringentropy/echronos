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

/**
 * Enable preemption, and in doing so, allow any pending preemptions to occur.
 */
.global rtos_internal_preempt_enable
/* void rtos_internal_preempt_enable(void); */
rtos_internal_preempt_enable:

/**
 * Disable preemption.
 */
.global rtos_internal_preempt_disable
/* void rtos_internal_preempt_disable(void); */
rtos_internal_preempt_disable:

/* This function returns 1 if preemption is disabled, and 0 otherwise.
 */
.global rtos_internal_check_preempt_disabled
/* bool rtos_internal_check_preempt_disabled(void); */
rtos_internal_check_preempt_disabled:

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