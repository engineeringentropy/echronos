/* Note: Some of this is cribbed from the ARMv7 architecture, but the ARMv6 is quite different in the 
 *   way that exceptions are handled.
 */

/**
 * Trigger a context-switch to the next task runnable as determined by the scheduler.
 * Intended to be used by the RTOS internally after taking actions that change the set of schedulable tasks.
 */
.global rtos_internal_yield
/* void rtos_internal_yield(void); */
rtos_internal_yield:
        /* Pre-emption is generally disabled when we get here.
         * A task that is yielding probably doesn't mind being preempted.
         * We can't call svc with pre-emption disabled, but we can't disable pendsv easily.
         * Solution seems to be enable pre-emption (no other datastructures are being touched)
         *   and then do a synchronous yield with svc
         */
        cpsie i
        svc #0
        cpsid i
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
@     uint32_t runFlags; /* 64 */
@ };

@ void rtos_internal_context_switch_first(context_t *);
@ */
.global rtos_internal_context_switch_first
rtos_internal_context_switch_first:
        /* On entry, we're on the main stack and in thread mode, so set the new stack and switch the stack
         * We don't need to set LR or anything else.
         * Interrupts are disable here, we don't enable them.
         * Interrupts are enabled during the entry_{task} function.
         *
         */
        /* Put the stack pointer into psp */
        ldr r1, [r0]
        add r1, #68
        msr psp, r1
        /* #56 comes from samd21_exception_context_stack_t */
        ldr r1, [r0]
        ldr r1, [r1,#56]
        /* Clean up our stack to the top
         * Note that this is the size of the SP in bytes!
         * This must be the same as MSPStack in samd21-exceptions.c
         * This is legal because rtos_start doesn't return.
         * Transitively, this means that this function does not return.
         * Note that add for sp can only be 0-508, so we split this in two.
         */
        ldr r0, =MSPStack
        mov sp, r0
        add sp, #508
        add sp, #4
        /* Switch stacks */
        mrs r2, control
        mov r0, #2
        orr r2, r0
        msr control, r2

        /* Start executing the task */
        mov pc, r1

.global rtos_internal_svc_handler
.thumb_func
rtos_internal_svc_handler:
/* Implements the functionality of rtos_internal_yield. */
        /* Determine if a context switch is needed, or bail from this handler */
        /* Call the internal scheduling algorithm. The /TaskId/ of the task to be switched to ends up in r0. */
        /* new_task<r0> = interrupt_event_get_next() */

        /* r0 = interrupt_event_get_next */
        mov r1, ip
        mov r2, lr
        push {r1-r2}
        bl rtos_internal_interrupt_event_get_next
        pop {r1-r2}
        mov ip, r1
        mov lr, r2
        mov r2, r0

        /* r1 = (uint8_t)rtos_internal_current_task */
        ldr r0, =rtos_internal_current_task
        ldrb r1, [r0]

        /* if (old_task<r1> == new_task<r0>) return */
        cmp r2, r1
        beq 1f

        /* Perform the context switch 
         * We have the old taskid in r1, and the new task id in r2
         * Rough algorithm:
         *  - load a pointer to rtos_internal_tasks, which is defined as struct task rtos_internal_tasks[{{tasks.length}}];
         *  - dereference pointer to retrieve a pointer to samd21_task_context_layout_t for the old task
         *  - dereference pointer to retrieve a pointer to samd21_task_context_layout_t for the new task
         *  - save r4-r11 into the old stack
         *  - retrieve r4-11 from the new stack
         *  - change psp to the new stack
         */
        
        lsl r1, #2
        ldr r0, =rtos_internal_tasks
        mrs r3, psp
        /* r3 = pointer to samd21_task_context_layout_t for the old task */        
        /* Store the old context into the stack */
        sub r3, #36
        str r3, [r0, r1]
        ldr r0, #0
        stmia r3!, {r0,r4-r7}
        mov r4, r8
        mov r5, r9
        mov r6, r10
        mov r4, r11
        stmia r3!, {r4-r7}

        /* Load the new context from the stack */
        lsl r2, #2
        ldr r0, =rtos_internal_tasks
        ldr r3, [r0, r2]
        add r3, #16
        ldmia r3!, {r4-r7}
        mov r8, r4
        mov r9, r5
        mov r10, r6
        mov r11, r7
        sub r3, #36 /* 16 + 4 * 4  + 4 */
        ldmia r3!, {r0, r4-r7}

        /* Set the new PSP stack 
         * r0 is a pointer to =rtos_internal_tasks
         */
        msr psp, r3

        /* Disable interrupts if the runflags bit0 is set 
         * Note that we've moved the stack pointer up by 32 bytes,
         *  so the +64 offset becomes +32
         */
        mov r1, #1
        and r1, r0
        cmp r1, #0
        beq 1f
        /* Disable interrupts on return if this is the first time a thread is being run */
        cpsid i
        /* Reset the bit */
        ldr r1, =0xFFFFFFFE
        and r0, r1
        str r0, [r3, #32]
1:
        bx lr

.global rtos_internal_pendsv_handler
.thumb_func
rtos_internal_pendsv_handler:
        /* Clear PendSV to avoid a potentially nasty tail-chaining bug
         * see https://embeddedgurus.com/state-space/2011/09/whats-the-state-of-your-cortex/
         */
         /* Write  a 1 to ICSR in bit PENDSVCLR 
          * Can't use MRS/MSR for this for some reason.
          */
        ldr r0, =0xE000ED04
        ldr r1, =0x08000000
        str r1, [r0]

        /* Determine if a context switch is needed, or bail from this handler */
        /* Call the internal scheduling algorithm. The /TaskId/ of the task to be switched to ends up in r0. */
        /* new_task<r0> = interrupt_event_get_next() */

        /* r0 = interrupt_event_get_next */
        mov r1, ip
        mov r2, lr
        push {r1-r2}
        bl rtos_internal_interrupt_event_get_next
        pop {r1-r2}
        mov ip, r1
        mov lr, r2
        mov r2, r0

        /* r1 = (uint8_t)rtos_internal_current_task */
        ldr r0, =rtos_internal_current_task
        ldrb r1, [r0]

        /* if (old_task<r1> == new_task<r0>) return */
        cmp r2, r1
        beq 1f

                /* Perform the context switch 
         * We have the old taskid in r1, and the new task id in r2
         * Rough algorithm:
         *  - load a pointer to rtos_internal_tasks, which is defined as struct task rtos_internal_tasks[{{tasks.length}}];
         *  - dereference pointer to retrieve a pointer to samd21_task_context_layout_t for the old task
         *  - dereference pointer to retrieve a pointer to samd21_task_context_layout_t for the new task
         *  - save r4-r11 into the old stack
         *  - retrieve r4-11 from the new stack
         *  - change psp to the new stack
         */
        
        lsl r1, #2
        ldr r0, =rtos_internal_tasks
        mrs r3, psp
        /* r3 = pointer to samd21_task_context_layout_t for the old task */        
        /* Store the old context into the stack */
        sub r3, #36
        str r3, [r0, r1]
        ldr r0, #0
        stmia r3!, {r0,r4-r7}
        mov r4, r8
        mov r5, r9
        mov r6, r10
        mov r4, r11
        stmia r3!, {r4-r7}

        /* Load the new context from the stack */
        lsl r2, #2
        ldr r0, =rtos_internal_tasks
        ldr r3, [r0, r2]
        add r3, #16
        ldmia r3!, {r4-r7}
        mov r8, r4
        mov r9, r5
        mov r10, r6
        mov r11, r7
        sub r3, #36 /* 16 + 4 * 4  + 4 */
        ldmia r3!, {r0, r4-r7}

        /* Set the new PSP stack 
         * r0 is a pointer to =rtos_internal_tasks
         */
        msr psp, r3

        /* Disable interrupts if the runflags bit0 is set 
         * Note that we've moved the stack pointer up by 32 bytes,
         *  so the +64 offset becomes +32
         */
        mov r1, #1
        and r1, r0
        cmp r1, #0
        beq 1f
        /* Disable interrupts on return if this is the first time a thread is being run */
        cpsid i
        /* Reset the bit */
        ldr r1, =0xFFFFFFFE
        and r0, r1
        str r0, [r3, #32]
1:
        bx lr

