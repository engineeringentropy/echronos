/* Note: Cribbed from the ARMv7 implementation
 */

/* The Base Priority Mask Register (BASEPRI) is used to define the minimum priority for exception processing.
 * When set to a nonzero value, it prevents the activation of all exceptions with the same or lower priority level.
 * Note that lower priority levels are expressed by *higher* priority values.
 *
 * Setting BASEPRI to 0 enables all configurable-priority exceptions. */
.macro asm_preempt_enable scratch
        ldr \scratch, =#0
        msr basepri, \scratch
.endm

/* Setting BASEPRI to the same priority value as that of a configurable-priority interrupt disables the interrupt. */
.macro asm_preempt_disable scratch
        /* This value will match the PendSV priority used in the ARM-specific preempt_init function. */
        ldr \scratch, =#{{rtos.pendsv_priority}}
        msr basepri, \scratch
.endm

/* This macro is used by the preemption handler to clear the 'preemption pending' status. */
.macro asm_preempt_clear scratch0 scratch1
        /* Clear the PendSV bit in the ICSR (Interrupt Control and State Register) */
        ldr \scratch0, =0xE000ED04
        ldr \scratch1, =0x08000000
        str \scratch1, [\scratch0]
.endm

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
 *
 * Since the activation of pending PendSV interrupts (after enabling by resetting BASEPRI) is subject to a delay whose
 * length does not appear to be specified by any documentation, we explicitly await that any pending preemption has
 * been handled (by checking that the PendSV pending bit has been cleared by our PendSV handler) before returning.
 */
.global rtos_internal_preempt_enable
/* void rtos_internal_preempt_enable(void); */
rtos_internal_preempt_enable:
        asm_preempt_enable r0

1:      /* Ensure PendSV bit in the ICSR (Interrupt Control & State Register) has been cleared before proceeding. */
        ldr r0, =0xE000ED04
        ldr r1, [r0]
        tst r1, #0x10000000
        bne 1b

        bx lr

/**
 * Disable preemption.
 */
.global rtos_internal_preempt_disable
/* void rtos_internal_preempt_disable(void); */
rtos_internal_preempt_disable:
        asm_preempt_disable r0
        bx lr

/* This function returns 1 if preemption is disabled, and 0 otherwise.
 * We consider preemption 'disabled' if either of two things currently hold:
 * 1. We are currently in *any* handler (i.e. the IPSR, or equivalently PSR[8:0] or ICSR[8:0], is non-zero).
 *    This implies preemption is disabled, assuming we set PendSV to be the lowest priority level (i.e. highest
 *    priority value) interrupt handler, meaning that PendSV can preempt no other handler.
 * 2. The BASEPRI is raised to the priority of PendSV, which prevents PendSV from activating. */
.global rtos_internal_check_preempt_disabled
/* bool rtos_internal_check_preempt_disabled(void); */
rtos_internal_check_preempt_disabled:
        /* If the IPSR is non-zero, we're in another handler and so PendSV cannot preempt it */
        mrs r0, ipsr
        cbnz r0, 1f
        /* If the BASEPRI is non-zero, we've set it to PendSV's priority and so PendSV cannot occur */
        mrs r0, basepri
        cbz r0, 2f
1:
        mov r0, #1
2:
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